
/************************************Includes***************************************/
#include "../G8RTOS/G8RTOS_Scheduler.h"
#include "../G8RTOS/G8RTOS_IPC.h"

#include "../MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "bb_mania_threads.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define S_MAX 50
#define JOY_U_BOUND (2048+250)
#define JOY_L_BOUND (2048-250)
#define PIX_OFFSET 5
#define GRID_W 24
#define GRID_L 28
#define PIX_SQU 10
/*************************************Defines***************************************/


/*********************************Global Variables**********************************/
// define location of person with enum
typedef enum loc{
    SNAKE = 0, 
    EMPTY = 1, 
    ORANGE = 2
} loc; 

// define direction of snake with enum
typedef enum dir{
    UP = 0, 
    DOWN = 1, 
    LEFT = 2, 
    RIGHT = 3, 
    NONE = 4
} dir;

// struct for the point a player / object is at 
typedef struct Point{
    int16_t row; 
    int16_t col;
} Point; 

typedef struct Player{
    bool ball_contacted;
    Point current_point;
} Player;


/*********************************Global Variables**********************************/
// define global square entity based on block struct
// static Block square;
// static Block orange;
// Snake snake;
Player players[2];
static bool game_begin = true;
static bool game_over = false;
// static uint16_t joy_data_x;
// static uint16_t joy_data_y;
// loc game_array[X_MAX][Y_MAX];

/*********************************** FUNCTIONS ********************************/

// Prototypes
void draw_player(Player* playerx, int16_t color);
void reset_position(Player* playerx, uint8_t p_inx);




// Definitions
void draw_player(Player* playerx, int16_t color){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle((*playerx).current_point.col, (*playerx).current_point.row, 10, 70, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void reset_position(Player* playerx, uint8_t p_inx){
    if(p_inx == 0){
        playerx->current_point.col = 30;
        playerx->current_point.row = 10;
    }
    else if(p_inx == 1){
        playerx->current_point.col = 210;
        playerx->current_point.row = 10;
    }
}


// static inline dir opposite_dir(dir opp){
//     switch(opp){
//         case RIGHT : return LEFT; 
//         case LEFT  : return RIGHT; 
//         case UP    : return DOWN; 
//         case DOWN  : return UP;
//         default    : return NONE;
//     }
// }

/*************************************Threads***************************************/
// Working Threads 
void Idle_Thread_BB(void) {
    for(;;){
    }
}

void Game_Init_BB(void){
    for(;;){
        if(game_begin){

            reset_position(&players[0], 0);            
            reset_position(&players[1], 1);
        
            ST7789_DrawRectangle(0, 0, Y_MAX, 10, ST7789_WHITE);
   
            draw_player(&players[0], ST7789_GREEN);
            draw_player(&players[1], ST7789_BLUE);
            ST7789_ORANGE
            
            // sleep just in case
            sleep(10);

            game_begin = false;
        }
    }
}

// void Game_Update(void){
//     if(!game_over){
//         draw_block(&snake.snake_array[snake.tail_index], ST7789_BLACK);
//         update_snake();
//         move_snake();
//         check_lose();
//         check_collision();
//         draw_block(&snake.snake_array[snake.head_index], ST7789_WHITE);
//     }
// }

// // periodic thread
// void Get_Joystick_Snake(void) {

//     joy_data_x = JOYSTICK_GetX();
//     joy_data_y = JOYSTICK_GetY();
//     if(!game_over){
//         dir proposed = snake.snk_dir;
    
//         if(joy_data_x <= JOY_L_BOUND){
//             proposed = RIGHT;
//         }
//         else if(joy_data_x >= JOY_U_BOUND){
//             proposed = LEFT;
//         }
//         else if(joy_data_y <= JOY_L_BOUND){
//             proposed = DOWN;
//         }
//         else if(joy_data_y >= JOY_U_BOUND){
//             proposed = UP;
//         }

//         if(proposed != opposite_dir(snake.snk_dir)){
//             next_dir = proposed;
//         }
//     }
 
// }

// void Game_Over(void){
//     for(;;){
//         G8RTOS_WaitSemaphore(&sem_SPI);
//         ST7789_DrawRectangle(orange.current_point.col, orange.current_point.row, 10, 10, ST7789_BLACK);
//         for(int8_t i = 0; i < snake.snake_size; ++i){
//             ST7789_DrawRectangle(snake.snake_array[i].current_point.col, snake.snake_array[i].current_point.row, 10, 10, ST7789_BLACK);
//         }
//         G8RTOS_SignalSemaphore(&sem_SPI);
//         G8RTOS_KillSelf();
//     }
// }

// void Restart_Game(void){
//     for(;;){
//         G8RTOS_WaitSemaphore(&sem_JOY);
//         sleep(50);
//         uint32_t data = GPIOPinRead(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//         if(data == 0 && game_over){
//             // toggle joystick flag value
//             game_begin = true; 
//             game_over = false; 
//         }
//         GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//         GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//         sleep(10);
//     }
// }

// void Snake_GPIOD_Handler(void) {
//     GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//    	GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//     G8RTOS_SignalSemaphore(&sem_JOY);
// }

void Idle_Thread_Periodic_BB(void){
    // do nothing
}

