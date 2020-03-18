#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define XMAX 15
#define YMAX 19
#define INPUT_DELAY 20000
#define PAINT_DELAY 400000

int CHECK = 1;
int game_field[XMAX+2][YMAX+1];

void clrscr(void);
void print_grid(int X_MAX, int Y_MAX);
int  CheckRotated(void);
void RotateTetramino(void);
int  CheckTetramino(void);
void StepDownTetramino(void);
void PaintTetramino(void);

enum direction_enum{left = 1, right = 2} direction;


struct Point{
	int X;
	int Y;
};

struct TetraminoStruct{
    struct Point blocks[4];
    int type;
    int state;
} tetramino, prev_tetramino, temp_tetramino;



void MoveTetramino(void);



void PrintGameField()
{
	int i,j;

	printf("\033[%d;%dH",XMAX+3, 0);

    for (i=0;i<=(XMAX+1);i++){
        for (j=0;j<=YMAX;j++){
            printf("%d",game_field[i][j]);
        }
        printf("  %d\n",i);
    }
}



void InitGameField()
{
    int i,j;
    for (i=0;i<=(XMAX);i++)
        for (j=0;j<=YMAX;j++){
            if ((j==0)||(j==YMAX))
                game_field[i][j] = 1;
            else
                game_field[i][j] = 0;   // All fields empty,
        }       
    
    for (j=0;j<=YMAX;j++)
        game_field[i][j] = 1;       // but not last line
    
    PrintGameField();
}



void RefreshGameField(void)
{
	int i,j,k;

	if (CHECK == 2){	// if there is strike lines, delete it and move each other line down 1 step

		for (i=1; i<=XMAX; i++){
			if (game_field[i][1] == 5){		// find line with strike
				for (k = i; k>1; k--)
					for (j=1; j<YMAX; j++) 
						game_field[k][j] = game_field[k-1][j];
			}
		}

		for (i=1; i<=XMAX; i++){
			printf("\033[%d;%dH",i, 2);
			for (j=1; j<YMAX; j++) {
				if (game_field[i][j]==0)
					printf(" ");
				else
					printf("X");
			}
		}
		CHECK = 1;
	}

	else {			// No strike lines
		for (i=0;i<4;i++)
			game_field[tetramino.blocks[i].X][tetramino.blocks[i].Y-1] = 1;
	}

	PrintGameField();
}



void GenerateTetramino(void)
{
    //int number = rand()%2;
    // TODO: random figure number in future
    int number = 1;
    
    if (number == 1){   // L figure
        tetramino.blocks[0].X = 1;
        tetramino.blocks[0].Y = 10;
        tetramino.blocks[1].X = 2;
        tetramino.blocks[1].Y = 10;
        tetramino.blocks[2].X = 3;
        tetramino.blocks[2].Y = 10;
        tetramino.blocks[3].X = 3;
        tetramino.blocks[3].Y = 11;
        tetramino.type = 1;
    }

    tetramino.state = 1;
}



int mygetch( ) {
	struct termios oldt,
	newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}


//---------------------------------------------
//	Вспомогательная функция - обработка нажатий
//---------------------------------------------
void* inputThreadFunc(void* arg){
	int ch;

	while(CHECK){
		ch = mygetch();

		if (ch=='p'){
			CHECK=0;
			break;
		}

		else if (ch == 'w'){
			RotateTetramino();
		}

		else if (ch == 's'){
			if (CheckTetramino()){
				prev_tetramino = tetramino;
				StepDownTetramino();
				PaintTetramino();
			}
		}

		else if (ch == 'a'){
			direction = left;
			MoveTetramino();
		}

		else if (ch == 'd'){
			direction = right;
			MoveTetramino();
		}

		usleep(INPUT_DELAY);	
	}
    return NULL;
}
//---------------------------------------------



void PaintTetramino(void)
{
    int i;
    // Clear previous tetramino
    for (i=0; i<4; i++){
        printf("\033[%d;%dH ",prev_tetramino.blocks[i].X,prev_tetramino.blocks[i].Y);
    }

    printf("\033[%d;%dH                                               ", 10, (YMAX+10));
    // Paint new tetramino
    for (i=0; i<4; i++){       
        printf("\033[%d;%dH%d,%d;", 10, (YMAX+10+i*7), tetramino.blocks[i].X, tetramino.blocks[i].Y);
        //printf("\033[%d;%dHX",tetramino.blocks[i].X,tetramino.blocks[i].Y);
        //printf("\033[%d;%dH\u25a2",tetramino.blocks[i].X,tetramino.blocks[i].Y);
        printf("\033[%d;%dH@",tetramino.blocks[i].X,tetramino.blocks[i].Y);
    }
}



int CheckTetramino(void)
{
    int i;
    int ok = 1;

    for (i=0;i<4;i++){
    	// check for free space under tetramino
        if (game_field[tetramino.blocks[i].X+1][tetramino.blocks[i].Y-1] == 1){
            ok = 0;
            // if no free space and first line - GAME OVER
            if (tetramino.blocks[i].X==1)
            	CHECK = 0;
        }

        // if generate figure born on the existing block - GAME OVER
        if (game_field[tetramino.blocks[i].X][tetramino.blocks[i].Y-1] == 1)
        	CHECK = 0;

    }

    return ok;
}



void StepDownTetramino(void)
{
    int i;
    for (i=0;i<4;i++){
        (tetramino.blocks[i].X)++;
    }
}



int CheckForLines(void)
{
	int i,j;
	int return_value = 0;
	int filled_line;

	for (i=1; i<=XMAX; i++){
		int filled_line = 1;
		for (j=1; j<YMAX; j++){
			if (game_field[i][j]==0)
				filled_line = 0;
		}
		if (filled_line){	// all line is filled
			return_value = 1;
			for (j=1; j<YMAX; j++)
				game_field[i][j] = 5;
		}
	}

	return return_value;
}



void MoveTetramino(void)
{
	int ok = 1;
	int i;
	
	if (direction == left){
		for (i=0;i<4;i++){
			if (game_field[tetramino.blocks[i].X][tetramino.blocks[i].Y-2] == 1)
				ok = 0;
		}

		if (ok){
			prev_tetramino = tetramino;	
			for (i=0;i<4;i++)
				tetramino.blocks[i].Y--;
			PaintTetramino();
		}
	}

	else if (direction == right){
		for (i=0;i<4;i++){
			if (game_field[tetramino.blocks[i].X][tetramino.blocks[i].Y] == 1)
				ok = 0;
		}

		if (ok){
			prev_tetramino = tetramino;	
			for (i=0;i<4;i++)
				tetramino.blocks[i].Y++;
			PaintTetramino();
		}
	}

}



void RotateTetramino(void)
{
	int i;
	temp_tetramino = tetramino;

	if (tetramino.type == 1){	// L type tetramino
		if (tetramino.state == 1){
			temp_tetramino.blocks[0].X++;
			temp_tetramino.blocks[0].Y--;
			temp_tetramino.blocks[2].X--;
			temp_tetramino.blocks[2].Y++;
			temp_tetramino.blocks[3].X -= 2;
			temp_tetramino.state = 2;
		}
		else if (tetramino.state == 2){
			temp_tetramino.blocks[0].X++;
			temp_tetramino.blocks[0].Y++;
			temp_tetramino.blocks[2].X--;
			temp_tetramino.blocks[2].Y--;
			temp_tetramino.blocks[3].Y -= 2;
			temp_tetramino.state = 3;
		}
		else if (tetramino.state == 3){
			temp_tetramino.blocks[0].X--;
			temp_tetramino.blocks[0].Y++;
			temp_tetramino.blocks[2].X++;
			temp_tetramino.blocks[2].Y--;
			temp_tetramino.blocks[3].X += 2;
			temp_tetramino.state = 4;
		}
		else if (tetramino.state == 4){
			temp_tetramino.blocks[0].X--; 
			temp_tetramino.blocks[0].Y--;
			temp_tetramino.blocks[2].X++;
			temp_tetramino.blocks[2].Y++;
			temp_tetramino.blocks[3].Y += 2;
			temp_tetramino.state = 1;
		}
	}

	if (CheckRotated()){
		prev_tetramino = tetramino;
		tetramino = temp_tetramino;
		PaintTetramino();
	}
}



// Check rotated tetramino for collision with existing blocks and walls
int CheckRotated(void)
{
	int i;
	int return_value = 1;

	for (i=0; i<4; i++)
		if (game_field[temp_tetramino.blocks[i].X][temp_tetramino.blocks[i].Y-1] == 1)
			return_value = 0;

	return return_value;
}



void PrintStrike(void)
{
	int i,j;
	for (i=1;i<=XMAX;i++){
		if (game_field[i][1] == 5){
			printf("\033[%d;%dH",i, 2);
			for (j=1; j<YMAX; j++)
				printf("=");
		}
	}
}



//-----------------------------------
//	Основная функция - отрисовка
//-----------------------------------
void* thread_func(void* arg)
{
    int x,y;
    int i,j;
    int i_counter, j_counter;

    while(CHECK){

    	if (CHECK == 2){
    		RefreshGameField();//TODO: Paint game field with moved down lines, and return to normal state 
    		// 			+ paint new tetramino, generated in previous iteration
    	}

    	if (CheckTetramino() == 1){	// there is free space under tetramino
    		prev_tetramino = tetramino;
    		StepDownTetramino();
    		PaintTetramino();
    	}

    	else {						// no free space under tetramino(collision)
    		RefreshGameField();

    		if (CheckForLines()){	// If there is strike lines
    			CHECK = 2;
    			PrintGameField();
    			PrintStrike();
    			GenerateTetramino();
	    		prev_tetramino = tetramino;
    		}
			else {					// no strike lines, just refresh 
	    		GenerateTetramino();
	    		prev_tetramino = tetramino;
	    		PaintTetramino();
	    	}
    	}


    	fflush(stdout);             // force clear console buffer
    	usleep(PAINT_DELAY);		// Sleep() until the next step
    }
    
    return NULL;
}
//-----------------------------------



int main()
{
	struct termios oldt,newt;
	pthread_t* tid;
	pthread_t* tid2;
	pthread_cond_t* cond;
	srand(time(0));

	// allocate memory to cond (conditional variable),  
    // thread id's and array of size threads 
    cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 
    tid  = (pthread_t*)malloc(sizeof(pthread_t)); 
    tid2 = (pthread_t*)malloc(sizeof(pthread_t)); 

	printf ("\e[?25l");	//set cursor INVISIBLE

	clrscr();
	print_grid(XMAX,YMAX);
    InitGameField();

    printf("\033[%d;%dH blocks:",10,YMAX+2);
    
    GenerateTetramino();
    prev_tetramino = tetramino;

	pthread_create(tid, NULL, thread_func, NULL);
	pthread_create(tid2, NULL, inputThreadFunc, NULL);
	
	while(1){
        if (CHECK == 0){
            pthread_cancel(*tid);
            pthread_cancel(*tid2);
            printf("\033[%d;%dH ",XMAX+20,25);
            printf("END OF GAME\n");
            break;
        }
	}

	// Force make console symbols visible after thread kill with getchar()
	tcgetattr( STDIN_FILENO, &oldt );
	oldt.c_lflag |= ( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

	printf ("\e[?25h");		//set cursor VISIBLE

	return 0;
}


void clrscr(void)
{
	printf("\033[2J"); 	/* Clear the entire screen. */
	printf("\033[0;0f");/* Move cursor to the top left hand corner*/
}


void print_grid(int X_MAX, int Y_MAX){
	int x,y;
	for (x=0;x<=X_MAX;x++){
		for (y=0;y<=Y_MAX;y++){
			if (x==X_MAX){
					printf("#");
			}
			else {
				if (y==0 || y==Y_MAX)
					printf("#");
				else
					printf(" ");	 
			}
		}
		printf("\n");
	}
}
