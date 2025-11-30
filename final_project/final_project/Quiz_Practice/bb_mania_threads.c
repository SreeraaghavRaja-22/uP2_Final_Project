
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
#define BALL_COLOR ST7789_ORANGE
#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 70
#define HOOP_WIDTH 10
#define HOOP_HEIGHT 110
#define NET_WIDTH 20
#define BALL_RAD  10
#define MAX_BOUNCE_HEIGHT 50
#define JOY_U_BOUND (2048+250)
#define JOY_L_BOUND (2048-250)



/*************************************Defines***************************************/
// I AM PLAYER 1

/*********************************Global Variables**********************************/

// define direction 
typedef enum dir{
    DOWN = 0, UP
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
    int16_t prev_x;
    int16_t prev_y;
    bool shoot_ball;
    int16_t max_height;
    dir ball_dir; 
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
static uint8_t slow = 0; 
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
void bounce_ball(void);


// Definitions
void draw_player(Player* playerx, int16_t color, int16_t x_pos){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(x_pos, (*playerx).current_point.row, PLAYER_WIDTH, PLAYER_HEIGHT, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void draw_hoop(Hoop* hoopx, uint8_t h_inx){
    int16_t net_x = 0;
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
    int16_t bball_l = bball.current_point.col; 
    int16_t bball_r = bball.current_point.col + BALL_RAD;
    int16_t p0_l = players[0].current_point.col; 
    int16_t p0_r = players[0].current_point.col + PLAYER_WIDTH; 
    int16_t p1_l = players[1].current_point.col; 
    int16_t p1_r = players[1].current_point.col + PLAYER_WIDTH;

    if(bball_l <= p0_r && bball_r >= p0_l){
        bball.is_held = true; 
        bball.held_by = 1; 
        bball.current_point.col = p0_r;
    }
    else if(bball_l <= p1_r && bball_r >= p1_l){
        bball.is_held = true; 
        bball.held_by = 2; 
        bball.current_point.col = p1_l - BALL_RAD;
    }
}

void bounce_ball(void){\
    // slow ball bounce somehow 
    if(++slow < 24){
        return;
    }
    slow = 0; 

    if(!bball.shoot_ball){
        bball.max_height = 50;      
    }
    else{
        bball.max_height = 200;
    }

    if(bball.ball_dir == DOWN){
        if(bball.current_point.row <= 10){
            bball.ball_dir = UP;
        }
        else if(bball.current_point.row - 40 >= 10){
            bball.current_point.row--;  
        }      
        else if(bball.current_point.row - 30 >= 10){
            bball.current_point.row -= 2;
        }
        else if(bball.current_point.row - 20 >= 10){
            bball.current_point.row -= 3;
        }
        else{
            bball.current_point.row -= 5;
        }
    }  
    else{
        if(bball.current_point.row + 10 >= bball.max_height){
            bball.ball_dir = DOWN;
        }
         else if(bball.current_point.row + 40 <= bball.max_height){
            bball.current_point.row += 5;  
        }      
        else if(bball.current_point.row + 30 <= bball.max_height){
            bball.current_point.row += 3;
        }
        else if(bball.current_point.row + 20 <= bball.max_height){
            bball.current_point.row += 2;
        }
        else{
            bball.current_point.row++;
        }
    }

    if(bball.current_point.row <= 10){
        bball.current_point.row = 10; 
    }
    else if(bball.current_point.row >= bball.max_height){
        bball.current_point.row = bball.max_height;
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
            bball.current_point.col = 120;
            bball.current_point.row = 50;
            bball.max_height = 50; 
            bball.shoot_ball = false; 
            bball.ball_dir = 0;

            reset_position(&players[0], 0);            
            reset_position(&players[1], 1);
        
            ST7789_DrawRectangle(0, 0, X_MAX, 10, GROUND_COLOR);
   
            draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
            draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);

            draw_hoop(&hoops[0], 0);
            draw_hoop(&hoops[1], 1);

            draw_ball(ST7789_ORANGE);
            bball.prev_x = bball.current_point.col;
            bball.shoot_ball = false; 

            game_begin = false;
        }

        sleep(10);
    }
}

void Update_Screen(void){
    for(;;){
        bball.prev_x = bball.current_point.col;
        bball.prev_y = bball.current_point.row; 
        draw_ball(ST7789_BLACK);

        update_char(&players[1], del_x);
        
        // if(bball.current_point.col != bball.prev_x || bball.current_point.row != bball.prev_y){
        //     draw_ball(ST7789_BLACK);
        // }

        check_ball_pos();
        bounce_ball();


        if(players[0].is_moved){
            draw_player(&players[0], BG_COLOR, players[0].prev_x);
            draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
            players[0].is_moved = false;
        }

        if(players[1].is_moved){
            draw_player(&players[1], BG_COLOR, players[1].prev_x);
            draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);
            players[1].is_moved = false;
        }

        draw_ball(BALL_COLOR);

        sleep(5);
    }
}

void Read_Button(void){
    for(;;){
        G8RTOS_WaitSemaphore(&sem_PCA9555);        

        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);

        // sleep for a bit
        sleep(15);

        G8RTOS_WaitSemaphore(&sem_I2CA);
        uint8_t data = MultimodButtons_Get();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        uint8_t data_not = ~data;

        if(data & SW1 == SW1){
            bball.shoot_ball = true; 
            bball.held_by = 0; 
            bball.is_held = false; 
        }
        else if(data & SW2 == SW2){
            // do nothing
        }
        else if(data & SW3 == SW3){
            // do nothing
        }
        else if(data & SW4 == SW4){
           // do nothing
        }

        //this helps prevent the pin from activating on a rising edge (weird issue I ran into)
        uint8_t released;
        do {
            G8RTOS_WaitSemaphore(&sem_I2CA);
            released = MultimodButtons_Get();
            G8RTOS_SignalSemaphore(&sem_I2CA);
            sleep(1);
        } while (~released & (SW1 | SW2 | SW3 | SW4));

        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);

        sleep(10);
    }
}

/********************** Periodic Threads *****************************/
void Move_Character(void){
    del_x = (int16_t)JOYSTICK_GetX();
}

void Move_Opp(void){
    int16_t opp_mov = (rand() % 3) - 1;
    update_opp(&players[0], opp_mov);
}

/********************** Periodic Threads *****************************/

void Button_Handler(void){
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555);
}