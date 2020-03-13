#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define XMAX 20
#define YMAX 40

struct Point{
	int X;
	int Y;
};

struct Tetramino{
    struct Point figure[4];
    Point position;
    int height;
    int weight;
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
}



int main()
{
	struct termios oldt,newt;
	pthread_t* tid;
	pthread_t* tid2;
	pthread_cond_t* cond;
	direction = 3;
	srand(time(0));

	snake[0].X = 5;
	snake[0].Y = 5;

	// allocate memory to cond (conditional variable),  
    // thread id's and array of size threads 
    cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 
    tid  = (pthread_t*)malloc(sizeof(pthread_t)); 
    tid2 = (pthread_t*)malloc(sizeof(pthread_t)); 

	printf ("\e[?25l");	//set cursor INVISIBLE

	clrscr();
	print_grid(XMAX,YMAX);
	fruit.X = 2+(rand()%(XMAX-1));
	fruit.Y = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit.X,fruit.Y);		// Paint new fruit
	printf("\033[%d;%dH ",snake[0].X,snake[0].Y);
	printf("\033[%d;%dH Total Score: 10", XMAX+2, 0);
	printf("\033[%d;%dH Press (p) key for exit", 2, YMAX+2);
	printf("\033[%d;%dH w,a,s,d - control", 3, YMAX+2);

	pthread_create(tid, NULL, thread_func, NULL);
	//pthread_create(tid2, NULL, inputThreadFunc, NULL);
	
	while(1){

		if (CHECK == 0){
			pthread_cancel(*tid);
			pthread_cancel(*tid2);
			printf("\033[%d;%dH ",XMAX+3,25);
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
