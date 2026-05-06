//--------------------------------------------------------
// Application demonstrator: SNAKE game
//--------------------------------------------------------


#include "EDK_CM0.h" 
#include "core_cm0.h"
#include "edk_driver.h"
#include "edk_api.h"

#include <stdio.h>

//Maximum snake length
#define N 200							

//Game region
#define left_boundary 5
#define right_boundary 96
#define top_boundary 5
#define bottom_boundary 116
#define boundary_thick 1		//changed to global variable
#define CANNON_OFFSET 2				// used for hit detection
#define PlayerWIDTH 3
#define PlayerHEIGHT 3
#define background_color BLACK

//Global variables
static int ceiling_thick = 1;	//changed to static variable
static int i, j;
static char key;
static int cannon_score;
static int boat_score;
static int pause;
static int player_has_moved;

static int tick_counter = 0;		//Used for speed and score keeping
static int seconds_elapsed = 0;

static int gamespeed;
static int speed_table[10]={6,9,12,15,20,25,30,35,40,100};

// Structure define
struct target{
	int x;
	int y;
	int reach;
	}target;

struct Player{
	int x;
	int y;
	int width;
	int height;
	int direction;
	int node;
	}player;


struct Cannon{
	int x;
	int y;
}cannon[8];

typedef enum BallState{
	WAIT = 0,
	INITIAL = 1,
	FIRING = 2
}BallStateType;

struct Cannonball{
	int x;
	int y;
	int fire;
	BallStateType state;	
}cannonball[8];





//---------------------------------------------
// Game
//---------------------------------------------


void Game_Init(void)
{	
	//Draw a game region
	int i;
	clear_screen();


	rectangle(top_boundary, right_boundary, bottom_boundary, left_boundary, background_color); //tried to fill screen blue, did not work set back to black for now 


	rectangle(left_boundary,top_boundary,right_boundary,top_boundary+boundary_thick,BLUE);
	rectangle(left_boundary,top_boundary,left_boundary+boundary_thick,bottom_boundary,BLUE);
	rectangle(left_boundary,bottom_boundary,right_boundary,bottom_boundary+boundary_thick,BLUE);
	rectangle(right_boundary,top_boundary,right_boundary+boundary_thick,bottom_boundary+boundary_thick,BLUE);	
	

	
	for(i = 0; i < 8; i++){
		cannon[i].x = left_boundary+2*(5*(i+1));
		cannon[i].y = bottom_boundary - 1;
		draw_cannon(cannon[i].x, cannon[i].y);
	}
	for(i = 0; i < 8; i++){
		cannonball[i].x = 0;
		cannonball[i].y = 0;
		cannonball[i].fire = 0;
		cannonball[i].state = INITIAL;

	}
	//Initialise data
	
	boat_score=0;							//set boat's score to 0
	cannon_score=20;						//set cannon score to 10 to start
	gamespeed=speed_table[boat_score];		//whenever boat gains a point, the game speed increases
	
	//Initialise timer (load value, prescaler value, mode value)
	timer_init((Timer_Load_Value_For_One_Sec/gamespeed),Timer_Prescaler,1);	
	timer_enable();
	
	target.reach=1;
	player.direction=1;
	player.x=46;player.y=32;
	player.width=PlayerWIDTH;player.height=PlayerHEIGHT;
	player.node=4;
	pause=0;

	draw_boat(player.x, player.y, player.width, player.height, BROWN_LIGHT);
	
	//Print instructions
	printf("\n\n-------- EDK Demo ---------");
	printf("\n---- Destroy the Boat!!! -----");
  printf("\nCentre btn ..... hard reset");
  printf("\nKeyboard r ..... soft reset");
  printf("\nKeyboard w ....... move Boat up");
  printf("\nKeyboard s ...... move Boat down");
  printf("\nKeyboard a ...... move Boat left");
  printf("\nKeyboard d ...... move Boat right");
	printf("\nShoot cannons using 8 rightmost FPGA switches");
  printf("\nKeyboard space ...... pause");
  printf("\n---------------------------");	
	printf("\nTo ran the game, make sure:");
	printf("\n*UART terminal is activated");
	printf("\n*UART baud rare:  19200 bps");
	printf("\n*Keyboard is in lower case");
  printf("\n---------------------------");
	printf("\nPress any key to start\n");	
	while(KBHIT()==0);
		
	printf("\nCannon_Score=%d\n",cannon_score);
	printf("\nBoat_Score=%d\n",boat_score);
	NVIC_EnableIRQ(Timer_IRQn);			//start timing
	NVIC_EnableIRQ(UART_IRQn);	
	NVIC_EnableIRQ(GPIO7_IRQn);
	NVIC_EnableIRQ(GPIO6_IRQn);
	NVIC_EnableIRQ(GPIO5_IRQn);
	NVIC_EnableIRQ(GPIO4_IRQn);
	NVIC_EnableIRQ(GPIO3_IRQn);
	NVIC_EnableIRQ(GPIO2_IRQn);
	NVIC_EnableIRQ(GPIO1_IRQn);
	NVIC_EnableIRQ(GPIO0_IRQn);
}


void Game_Close(void){
	clear_screen();
	cannon_score=0;
	boat_score=0;
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");		//flush screen
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	NVIC_DisableIRQ(Timer_IRQn);			
	NVIC_DisableIRQ(UART_IRQn);	
	NVIC_DisableIRQ(GPIO7_IRQn);
	NVIC_DisableIRQ(GPIO6_IRQn);
	NVIC_DisableIRQ(GPIO5_IRQn);
	NVIC_DisableIRQ(GPIO4_IRQn);
	NVIC_DisableIRQ(GPIO3_IRQn);
	NVIC_DisableIRQ(GPIO2_IRQn);
	NVIC_DisableIRQ(GPIO1_IRQn);
	NVIC_DisableIRQ(GPIO0_IRQn);	
}

	
int GameOver(void){
	char key;
	
	NVIC_DisableIRQ(UART_IRQn);
	NVIC_DisableIRQ(Timer_IRQn);
	NVIC_DisableIRQ(GPIO7_IRQn);
	NVIC_DisableIRQ(GPIO6_IRQn);
	NVIC_DisableIRQ(GPIO5_IRQn);
	NVIC_DisableIRQ(GPIO4_IRQn);
	NVIC_DisableIRQ(GPIO3_IRQn);
	NVIC_DisableIRQ(GPIO2_IRQn);
	NVIC_DisableIRQ(GPIO1_IRQn);
	NVIC_DisableIRQ(GPIO0_IRQn);	
	for(i=0; i < 8; i++){
		cannonball[i].x = 0;
		cannonball[i].y = 0;
		cannonball[i].fire = 0;
		cannonball[i].state = INITIAL;
	}

	printf("\nBOAT DESTROYED\n");
	printf("Boat_score: %d\n", boat_score);
	printf("Cannon_score: %d\n", cannon_score);
	printf("\nPress 'q' to quit");
	printf("\nPress 'r' to replay");
	while(1){
		while(KBHIT()==0);
		key = UartGetc();
		if (key == RESET){
			return 1;
		}
		else if (key == QUIT){	
			return 0;
		}
		else
			printf("\nInvalid input");
	}
		
}
//---------------------------------------------
// GPIO ISRs -- used to shoot cannons
//---------------------------------------------
void GPIO7_ISR()
{
	cannonball[7].fire = 1;
	//printf("ISR7 WORK\n");
}
void GPIO6_ISR()
{	
	cannonball[6].fire = 1;	
	//printf("ISR6 WORK\n");
}
void GPIO5_ISR()
{
	cannonball[5].fire = 1;	
	//printf("ISR5 WORK\n");
}
void GPIO4_ISR()
{
	cannonball[4].fire = 1;	
	//printf("ISR4 WORK\n");
}
void GPIO3_ISR()
{
	cannonball[3].fire = 1;	
	//printf("ISR3 WORK\n");
}
void GPIO2_ISR()
{
	cannonball[2].fire = 1;	
	//printf("ISR2 WORK\n");
}
void GPIO1_ISR()
{
	cannonball[1].fire = 1;
	//printf("ISR1 WORK\n");
}
void GPIO0_ISR()
{
	cannonball[0].fire = 1;
	//printf("ISR0 WORK\n");
}


//---------------------------------------------
// UART ISR -- used to input commands
//---------------------------------------------

void UART_ISR(void)
{
	key = UartGetc();

	if(key == UP) {
		clear_boat(player.x, player.y, player.width, player.height, background_color);
		player.y -= 2;
	}
	else if(key == DOWN) {
		clear_boat(player.x, player.y, player.width, player.height, background_color);
		player.y += 2;
	}
	else if(key == LEFT) {
		clear_boat(player.x, player.y, player.width, player.height, background_color);
		player.x -= 2;
	}
	else if(key == RIGHT) {
		clear_boat(player.x, player.y, player.width, player.height, background_color);
		player.x += 2;
	}
	else if(key == PAUSE) {
		if(pause == 0){
			pause = 1;
			NVIC_DisableIRQ(Timer_IRQn);
		}
		else{
			pause = 0;
			NVIC_EnableIRQ(Timer_IRQn);
		}
	}

	// this keeps the player inside the borders 
	if(player.x < left_boundary + boundary_thick)
		player.x = left_boundary + boundary_thick;
	
	if(player.x + player.width >= right_boundary)
		player.x = right_boundary - player.width - 1;
	
	if(player.y < top_boundary + boundary_thick)
		player.y = top_boundary + boundary_thick;
	
	if(player.y + player.height >= bottom_boundary)
		player.y = bottom_boundary - player.height - 1;

	if(player.y < top_boundary + ceiling_thick)
		player.y = top_boundary + ceiling_thick;
	
	
	draw_boat(player.x, player.y, player.width, player.height, BROWN_LIGHT);
		
}
 

//---------------------------------------------
// TIMER ISR -- used for changing game speed and score keeping
//---------------------------------------------


void Timer_ISR(void)
{
	
	int overlap;

	// If game is not paused
	if(pause==0){
		
		
        tick_counter++;     // Increment the tick counter

        // Check if 1 second has passed
        // Since Timer is initialized as (One_Sec / gamespeed), 
        // 1 second has passed when tick_counter == gamespeed.
        if(tick_counter >= gamespeed) {
            tick_counter = 0;
            seconds_elapsed++;
            
            if(seconds_elapsed % 5 == 0) {	//Every 5 seconds, boat score and speed is increased
                boat_score++;
								cannon_score--;
								ceiling_thick += 3;
								rectangle(left_boundary,top_boundary,right_boundary,top_boundary+ceiling_thick,RED);//ADDED
                if(boat_score < 10) {
                    gamespeed = speed_table[boat_score];
                    timer_init((Timer_Load_Value_For_One_Sec / gamespeed), Timer_Prescaler, 1);
                    timer_enable();
                }
                
                printf("\nSpeed Level: %d; Boat Score: %d\n", boat_score, boat_score);
            }
        }
    
			
			
		
			// Fire cannonball
			for(j = 0; j < 8; j++){
				if(cannonball[j].fire == 1){
					if(cannonball[j].state == FIRING){
						if(cannonball[j].y > top_boundary){
							move_cannonball(cannonball[j].x, cannonball[j].y, RED);
							cannonball[j].y--;
						}	
						else{
							cannonball[j].fire = 0;
							draw_cannonball(cannonball[j].x, cannonball[j].y, background_color);
							draw_cannonball(cannonball[j].x, cannonball[j].y+1, background_color);
							VGA_plot_pixel(cannonball[j].x, cannonball[j].y, BLUE);
							VGA_plot_pixel(cannonball[j].x-1, cannonball[j].y, BLUE);
							VGA_plot_pixel(cannonball[j].x+1, cannonball[j].y, BLUE);
							cannonball[j].state = INITIAL;
						}
					}
					else{
						cannonball[j].x = cannon[j].x;
						cannonball[j].y = cannon[j].y - 5;
						draw_cannonball(cannonball[j].x, cannonball[j].y, GREEN);
						cannonball[j].state = FIRING;
					}
					
				}
			}

			// Detect if Boat hits cannonball
			
				for(j=0; j<8; j++){
					if(	(cannonball[j].fire == 1) &&
							boat_hit_cannonball(
							player.x,
							player.y,
							player.width,
							player.height,
							cannonball[j].x,
							cannonball[j].y + CANNON_OFFSET)
					){
						printf("Conditions:\n");
						printf("%d\n", (player.x==cannonball[j].x&&player.y==(cannonball[j].y + CANNON_OFFSET) ));
						printf("%d\n", (player.x==cannonball[j].x+1&&player.y==(cannonball[j].y + CANNON_OFFSET) ));
						printf("%d\n", (player.x==cannonball[j].x-1&&player.y==(cannonball[j].y + CANNON_OFFSET)));
						printf("%d\n", (player.x==cannonball[j].x&&player.y==(cannonball[j].y+1 + CANNON_OFFSET)));
						printf("%d\n", (player.x==cannonball[j].x&&player.y==(cannonball[j].y-1 + CANNON_OFFSET)));
						printf("Variables:\n");
						printf("Cannonball.x: %d, Cannonball.y: %d, Cannonball.fire: %d\n", cannonball[j].x, cannonball[j].y, cannonball[j].fire);
						if (GameOver()==0)
							Game_Close();
						else
							Game_Init();
					}
				}

			

		}
	// Cannonball fire
			
	
	// Mark that snake has moved
	player_has_moved=1;

	//Display the total distance that the snake has moved
	Display_Int_Times();
		
	//Clear timer irq
	timer_irq_clear();
		
}	



//---------------------------------------------
// Main Function
//---------------------------------------------


int main(void){

	//Initialise the system
	SoC_init();
	//Initialise the game
	Game_Init();
	
	//Go to sleep mode and wait for interrupts
	while(1)
		__WFI();	
	

}


