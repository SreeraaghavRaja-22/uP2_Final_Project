
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

#define PLAYER1_COLOR ST7789_GREEN
#define PLAYER2_COLOR ST7789_BLUE
#define GROUND_COLOR ST7789_WHITE
#define BG_COLOR ST7789_BLACK
#define S_MAX 50
#define JOY_U_BOUND (2048+150)
#define JOY_L_BOUND (2048-150)
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
    bool is_moved;
    int16_t prev_x;
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
void draw_player(Player* playerx, int16_t color, int16_t x_pos);
void reset_position(Player* playerx, uint8_t p_inx);
void update_char(Player* playerx, int16_t del_x);




// Definitions
void draw_player(Player* playerx, int16_t color, int16_t x_pos){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(x_pos, (*playerx).current_point.row, 10, 70, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void reset_position(Player* playerx, uint8_t p_inx){
    if(p_inx == 0){
        playerx->current_point.col = 30;
        playerx->current_point.row = 10;
        playerx->prev_x = 30;
    }
    else if(p_inx == 1){
        playerx->current_point.col = 210;
        playerx->current_point.row = 10;
        playerx->prev_x = 210;
    }
}

void update_char(Player* playerx, int16_t del_x){
    playerx->prev_x = playerx->current_point.col;

    if(del_x >= JOY_U_BOUND){
        if(playerx->current_point.col - 10 > 20){
            playerx->current_point.col -= 10;
            playerx->is_moved = true;
        }
    }

    else if(del_x <= JOY_L_BOUND){
        if(playerx->current_point.col+10 < X_MAX - 20){
            playerx ->current_point.col += 10;
            playerx->is_moved = true; 
        }
    }
}


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
        
            ST7789_DrawRectangle(0, 0, X_MAX, 10, GROUND_COLOR);
   
            draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
            draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);

            game_begin = false;
        }

        sleep(10);
    }
}

void Update_Screen(void){
    for(;;){
        int16_t char_pos = G8RTOS_ReadFIFO(MOVEMENT_FIFO);

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("prev_x: %d\n\n", players[0].prev_x);
        G8RTOS_SignalSemaphore(&sem_UART);

        update_char(&players[0], char_pos);

        // if(players[0].is_moved){
        //     draw_player(&players[0], BG_COLOR, prev_x);
        //     draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
        //     players[0].is_moved = false;
        // }

        // G8RTOS_WaitSemaphore(&sem_UART);
        // UARTprintf("X Position: %d\n\n", players[0].current_point.col); 
        // G8RTOS_SignalSemaphore(&sem_UART);
               
        sleep(10);
    }
}

/********************** Periodic Threads *****************************/
void Idle_Thread_Periodic_BB(void){
    // do nothing
}

void Move_Character(void){
    int16_t del_x = (int16_t)JOYSTICK_GetX();
    int16_t fifo = G8RTOS_WriteFIFO(MOVEMENT_FIFO, del_x);
    G8RTOS_WaitSemaphore(&sem_UART);
    UARTprintf("X Position: %d\n\n", del_x);
    UARTprintf("FIFO Output: %d\n\n", fifo);
    G8RTOS_SignalSemaphore(&sem_UART);
}
/********************** Periodic Threads *****************************/
