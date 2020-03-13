#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define XMAX 15
#define YMAX 20

int CHECK = 1;
int direction;
int game_field[YMAX][XMAX+1];

void clrscr(void);
void print_grid(int X_MAX, int Y_MAX);

struct Point{
	int X;
	int Y;
};

struct TetraminoStruct{
    struct Point blocks[4];
//    struct Point position;
//    int height;
//    int weight;
} tetramino;


void InitGameField()
{
    int i,j;
    for (i=0;i<YMAX;i++)
        for (j=0;j<XMAX;j++)
            game_field[i][j] = 0;   // All fields empty,
    
    for (j=0;j<XMAX;j++)
        game_field[i][j] = 1;       // but not last line
}


void GenerateTetramino(void)
{
    //int number = rand()%2;
    int number = 1;
    
    if (number == 1){   // L figure
        tetramino.blocks[0].X = 1;
        tetramino.blocks[0].Y = 5;
        tetramino.blocks[1].X = 2;
        tetramino.blocks[1].Y = 5;
        tetramino.blocks[2].X = 3;
        tetramino.blocks[2].Y = 5;
        tetramino.blocks[3].X = 3;
        tetramino.blocks[3].Y = 6;
        
    }
    else if (number == 0){  // Square figure
        tetramino.blocks[0].X = 1;
        tetramino.blocks[0].Y = 5;
        tetramino.blocks[1].X = 1;
        tetramino.blocks[1].Y = 6;
        tetramino.blocks[2].X = 2;
        tetramino.blocks[2].Y = 5;
        tetramino.blocks[3].X = 2;
        tetramino.blocks[3].Y = 6;
    }
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



void* inputThreadFunc(void* arg){
	int ch;

	while(CHECK){
		ch = mygetch();

		if (ch=='p'){
			CHECK=0;
			break;
		}
		usleep(10000);	
	}
    return NULL;
}



void PaintTetramino(void)
{
    int i;
    for (i=0;i<4;i++){
        printf("\033[%d;%dHX",tetramino.blocks[i].X,tetramino.blocks[i].Y);
    }
}



void IncreaseTetramino(void)
{
    int i;
    for (i=0;i<4;i++){
        (tetramino.blocks[i].X)++;
    }
}



void* thread_func(void* arg)
{
    int x,y;
    int i,j;
    int i_counter, j_counter;

    while(CHECK){
        PaintTetramino();
        IncreaseTetramino();
        //printf("\033[%d;%dH",fruit.X,fruit.Y);
        //printf("in thread func()\n");
        fflush(stdout);            // force clear console buffer
        usleep(1000000);
    }
    
    return NULL;
}



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
    
    GenerateTetramino();

	pthread_create(tid, NULL, thread_func, NULL);
	pthread_create(tid2, NULL, inputThreadFunc, NULL);
	
	while(1){
        if (CHECK == 0){
            pthread_cancel(*tid);
            pthread_cancel(*tid2);
//            printf("\033[%d;%dH ",XMAX+3,25);
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
