
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
#define HOOP_WIDTH 10
#define HOOP_HEIGHT 110
#define NET_WIDTH 20
#define BALL_RAD  10
#define JOY_U_BOUND (2048+250)
#define JOY_L_BOUND (2048-250)



/*************************************Defines***************************************/
// I AM PLAYER 1

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

typedef struct Hoop{
    Point current_point;
    bool is_hit; 
    int16_t score; 
} Hoop;

typedef struct Ball{
    Point current_point; 
    bool is_held;
    uint8_t held_by;
} Ball; 


/*********************************Global Variables**********************************/
// define global square entity based on block struct
// static Block square;
// static Block orange;
// Snake snake;
Player players[2];
static bool game_begin = true;
static bool game_over = false;
int16_t del_x = 0;
Hoop hoops[2];
Ball bball;
// static uint16_t joy_data_x;
// static uint16_t joy_data_y;
// loc game_array[X_MAX][Y_MAX];

/*********************************** FUNCTIONS ********************************/

// Prototypes
void draw_player(Player* playerx, int16_t color, int16_t x_pos);
void draw_hoop(Hoop* hoopx, uint8_t h_inx);
void draw_ball(int16_t color);
void reset_position(Player* playerx, uint8_t p_inx);
void update_char(Player* playerx, int16_t del_x);
void update_opp(Player* playerx, int8_t del_x);
void check_ball_pos(void);


// Definitions
void draw_player(Player* playerx, int16_t color, int16_t x_pos){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(x_pos, (*playerx).current_point.row, 10, 70, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void draw_hoop(Hoop* hoopx, uint8_t h_inx){
    int16_t net_x = 0, net_y = 0;
    if(h_inx == 0){
        hoopx->current_point.row = 10; 
        hoopx->current_point.col = 10; 
        hoopx->is_hit = false;
        net_x = hoopx->current_point.col + HOOP_WIDTH;
    }
    else if(h_inx == 1){
        hoopx->current_point.row = 10; 
        hoopx->current_point.col = 220; 
        hoopx->is_hit = false;
        net_x = hoopx->current_point.col - NET_WIDTH;
    }
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(hoopx->current_point.col, hoopx->current_point.row, HOOP_WIDTH, HOOP_HEIGHT, ST7789_RED);
    ST7789_DrawRectangle(net_x, HOOP_HEIGHT, NET_WIDTH, HOOP_WIDTH, ST7789_RED);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void draw_ball(int16_t color){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(bball.current_point.col, bball.current_point.row, BALL_RAD, BALL_RAD, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void reset_position(Player* playerx, uint8_t p_inx){
    if(p_inx == 0){
        playerx->current_point.col = 20;
        playerx->current_point.row = 10;
        playerx->prev_x = 20;
        
    }
    else if(p_inx == 1){
        playerx->current_point.col = 210;
        playerx->current_point.row = 10;
        playerx->prev_x = 210;

    }
    playerx->is_moved = false;
}

void update_char(Player* playerx, int16_t del_x){
    playerx->prev_x = playerx->current_point.col;

    if(del_x > JOY_U_BOUND){
        if(playerx->current_point.col - 10 > 10){
            playerx->current_point.col--;
            playerx->is_moved = true;
        }
    }

    else if(del_x < JOY_L_BOUND){
        if(playerx->current_point.col + 10 < X_MAX - 20){
            playerx ->current_point.col++;
            playerx->is_moved = true; 
        }
    }
}

void update_opp(Player* playerx, int8_t del_x){
    playerx->prev_x = playerx->current_point.col;
    if(del_x < 0){
        if(playerx->current_point.col - 10 > 10){
            playerx->current_point.col += del_x * 10;
            playerx->is_moved = true;
        }
    }
    else if(del_x > 0){
        if(playerx->current_point.col + 10 < X_MAX - 20){
            playerx->current_point.col += del_x * 10;
            playerx->is_moved = true;
        }
    }  
}

void check_ball_pos(void){
    draw_ball(ST7789_BLACK);
    if(bball.current_point.col == players[0].current_point.col){
        bball.is_held = true; 
        bball.held_by = 1; 
    }
    else if(bball.current_point.col == players[1].current_point.col){
        bball.is_held = true; 
        bball.held_by = 2; 
    }
    else{
        bball.is_held = false; 
        bball.held_by = 0;
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
            bball.current_point.col = 115;
            bball.current_point.row = 50;

            reset_position(&players[0], 0);            
            reset_position(&players[1], 1);
        
            ST7789_DrawRectangle(0, 0, X_MAX, 10, GROUND_COLOR);
   
            draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
            draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);

            draw_hoop(&hoops[0], 0);
            draw_hoop(&hoops[1], 1);

            draw_ball(ST7789_ORANGE);

            game_begin = false;
        }

        sleep(10);
    }
}

void Update_Screen(void){
    for(;;){
        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("prev_x: %d\n\n", players[1].prev_x);
        G8RTOS_SignalSemaphore(&sem_UART);

        update_char(&players[1], del_x);
        
        check_ball_pos();

        if(players[1].is_moved){
            draw_player(&players[1], BG_COLOR, players[1].prev_x);
            draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);
            if(bball.held_by == 2){
                bball.current_point.col = players[1].current_point.col;
                draw_ball(ST7789_ORANGE);
            }
            players[1].is_moved = false;
        }

        if(players[0].is_moved){
            draw_player(&players[0], BG_COLOR, players[0].prev_x);
            draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
            if(bball.held_by == 1){
                bball.current_point.col = players[0].current_point.col;
                draw_ball(ST7789_ORANGE);
            }
            players[0].is_moved = false;
        }
    }
}

/********************** Periodic Threads *****************************/
void Idle_Thread_Periodic_BB(void){
    // do nothing
}

void Move_Character(void){
    del_x = (int16_t)JOYSTICK_GetX();
}

void Move_Opp(void){
    int16_t opp_mov = (rand() % 3) - 1;
    update_opp(&players[0], opp_mov);
}

void Update_Ball(void){
    draw_ball(ST7789_BLACK);

   

    draw_ball(ST7789_ORANGE);
}
/********************** Periodic Threads *****************************/
