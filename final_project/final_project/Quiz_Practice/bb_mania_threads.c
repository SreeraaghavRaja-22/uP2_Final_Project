
/************************************Includes***************************************/
#include "../G8RTOS/G8RTOS_Scheduler.h"
#include "../G8RTOS/G8RTOS_IPC.h"

#include "../MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "bb_mania_threads.h"
#include "SPI_string.h"
#include "bitmap.h"
/************************************Includes***************************************/

/*************************************Defines***************************************/
#define PLAYER1_COLOR ST7789_GREEN
#define PLAYER2_COLOR ST7789_BLUE
#define GROUND_COLOR ST7789_YELLOW
#define BG_COLOR ST7789_BLACK
#define BALL_COLOR ST7789_ORANGE
#define NET_COLOR ST7789_WHITE
#define SCOREBOARD_COLOR ST7789_LIGHTBL
#define SCOREBOARD_HEIGHT 40
#define SCORE_WIN_COUNT 11 
#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 50
#define HOOP_WIDTH 10
#define HOOP_HEIGHT 90
#define NET_LENGTH 20
#define NET_HEIGHT 70
#define NET_WIDTH 20
#define BALL_RAD  10
#define MAX_BOUNCE_HEIGHT 30
#define GROUND 10
#define JOY_U_BOUND (2048+250)
#define JOY_L_BOUND (2048-250)

/*************************************Defines***************************************/
// I AM PLAYER 2

/*********************************Global Variables**********************************/
// define direction 
typedef enum dir{
    RIGHT = 1, 
    LEFT = 2,
    UP = 3, 
    DOWN = 4
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
    uint8_t score; 
    uint8_t prev_score;
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
    int x; 
    int y; 
    int vx;
    int vy;
    bool airborne;  
} Ball; 

/*********************************Global Variables**********************************/
Player players[2];
static bool game_begin = false; // false until start screen ends
static bool game_over = false;
static bool start_screen = true; 
static bool go_start = false;
static bool display_start = true;
int16_t del_x = 0;
Hoop hoops[2];
Ball bball;
static uint8_t slow = 0; 
char P1_BUFF[32];
char P2_BUFF[32];
dir p1 = RIGHT; 
dir p2 = LEFT;

extern const uint16_t ballSprite[10][10];
extern const uint16_t netSprite[10][20];
extern const uint16_t michael_jordan[50][10];


/*********************************** FUNCTIONS ********************************/
// Prototypes
void draw_player(Player* playerx, int16_t color, int16_t x_pos);
void draw_hoop(Hoop* hoopx, uint8_t h_inx);
void draw_hoop_sprite(Hoop* hoopx, uint8_t h_inx);
void draw_ball(int16_t color);
void reset_position(Player* playerx, uint8_t p_inx);
void reset_players(void);
void update_char(Player* playerx, int16_t del_x);
void update_opp(Player* playerx, int8_t del_x);
void check_ball_pos(void);
void boundary_cond(void);
void physics_update(void);
void shoot_logic(void);
void pickup_ball(void);
void throw_logic(void);
void bounce_ball(void);
void ball_movement(void);
void check_ball_hoop(void);
void reset_ball(void);
void draw_scoreboard(void);
void check_win(void);
void update_score(void);
void draw_ball_sprite(void);


// Definitions
void draw_player(Player* playerx, int16_t color, int16_t x_pos){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(x_pos, (*playerx).current_point.row, PLAYER_WIDTH, PLAYER_HEIGHT, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void draw_player0_sprite(void){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    for(int i = 0; i < PLAYER_HEIGHT; i++){
        for(int j = 0; j < PLAYER_WIDTH; j++){
            if(michael_jordan[PLAYER_HEIGHT - 1 - j][i] != ST7789_BLACK){
                ST7789_DrawPixel(players[1].current_point.col + j, players[1].current_point.row + i, michael_jordan[PLAYER_HEIGHT - 1 - j][i]);
            }
        }
    }
    G8RTOS_SignalSemaphore(&sem_SPIA);
}
void draw_hoop(Hoop* hoopx, uint8_t h_inx){
    int16_t net_x = 0;
    if(h_inx == 0){
        hoopx->current_point.row = 10; 
        hoopx->current_point.col = 0; 
        hoopx->is_hit = false;
        net_x = hoopx->current_point.col + HOOP_WIDTH;
    }
    else if(h_inx == 1){
        hoopx->current_point.row = 10; 
        hoopx->current_point.col = 230; 
        hoopx->is_hit = false;
        net_x = hoopx->current_point.col - NET_LENGTH;
    }
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(hoopx->current_point.col, hoopx->current_point.row, HOOP_WIDTH, HOOP_HEIGHT, ST7789_RED);
    ST7789_DrawRectangle(net_x, NET_HEIGHT, NET_LENGTH, NET_WIDTH, NET_COLOR);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}
void draw_hoop_sprite(Hoop* hoopx, uint8_t h_inx){
    int16_t net_x = 0;
    if(h_inx == 0){
        hoopx->current_point.row = 10; 
        hoopx->current_point.col = 0; 
        hoopx->is_hit = false;
        net_x = hoopx->current_point.col + HOOP_WIDTH;
    }
    else if(h_inx == 1){
        hoopx->current_point.row = 10; 
        hoopx->current_point.col = 230; 
        hoopx->is_hit = false;
        net_x = hoopx->current_point.col - NET_LENGTH;
    }

    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(hoopx->current_point.col, hoopx->current_point.row, HOOP_WIDTH, HOOP_HEIGHT, ST7789_GRAY);
    
    for(int i = 0; i < NET_WIDTH; i++){
        for(int j = 0; j < NET_LENGTH; j++){
           if(netSprite[NET_WIDTH - 1 - i][j] != ST7789_BLACK){
            ST7789_DrawPixel(net_x + j, NET_HEIGHT + i, netSprite[NET_WIDTH - 1 - i][j]);
           }
        }
    }
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

void draw_ball(int16_t color){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(bball.current_point.col, bball.current_point.row, BALL_RAD, BALL_RAD, color);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}
void draw_ball_sprite(void){
    G8RTOS_WaitSemaphore(&sem_SPIA);
    for(int i = 0; i < BALL_RAD; i++){
        for(int j = 0; j < BALL_RAD; j++){
            if(ballSprite[BALL_RAD - 1 - j][i] != ST7789_BLACK){
                ST7789_DrawPixel(bball.current_point.col + j, bball.current_point.row + i, ballSprite[BALL_RAD - 1 - j][i]);
            }
        }
    }
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
void reset_players(void){
    draw_player(&players[0], BG_COLOR, players[0].current_point.col);
    draw_player(&players[1], BG_COLOR, players[1].prev_x);
    reset_position(&players[0], 0);
    reset_position(&players[1], 1);
    draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);
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

    if(bball_l <= p0_r && bball_r >= p0_l && !bball.shoot_ball){
        bball.is_held = true; 
        bball.held_by = 1; 
        bball.current_point.col = p0_r;
    }
    else if(bball_l <= p1_r && bball_r >= p1_l && !bball.shoot_ball){
        bball.is_held = true; 
        bball.held_by = 2; 
        bball.current_point.col = p1_l - BALL_RAD;
    }
    else{
        bball.is_held = false;
        if(!bball.shoot_ball)
        bball.held_by = 0;
    }

    if(!bball.is_held && !bball.shoot_ball){
          // boundary conditions
        if(bball.current_point.col + BALL_RAD >= hoops[1].current_point.col){
            bball.current_point.col = hoops[1].current_point.col - BALL_RAD;
        }
        if(bball.current_point.col <= hoops[0].current_point.col){
            bball.current_point.col = hoops[0].current_point.col;
        }
    }
}
void boundary_cond(void){
    if(bball.current_point.col <= 20){
        bball.current_point.col = 20;
    }
    else if(bball.current_point.col >= 210){
        bball.current_point.col = 210;
    }
}
void physics_update(void){
    if(!bball.airborne)
        return; 
    
    bball.vy -= 1; 
    bball.current_point.col += bball.vx; 
    bball.current_point.row += bball.vy;

    if(bball.current_point.row <= GROUND){
        bball.current_point.row = GROUND;

        bball.vy = -(bball.vy >> 1); //

        if(abs(bball.vy) < 1){
            bball.airborne = false; 
            bball.vy = 0;
            bball.vx = 0;
        }
    }
}
void shoot_logic(void){
    if(bball.is_held && bball.shoot_ball){
        bball.vx = (bball.held_by == 1) ? +2 : -2; 
        bball.vy = 15;
        bball.airborne = true; 
        bball.is_held = false;
        bball.shoot_ball = false;
    }
}
void pickup_ball(void){    
    for(int i = 0; i < 2; i++){
        int bL = bball.current_point.col; 
        int bR = bball.current_point.col + BALL_RAD; 
        int bB = bball.current_point.row;
        int bT = bball.current_point.row + BALL_RAD;

        int pL = players[i].current_point.col;
        int pR = players[i].current_point.col + PLAYER_WIDTH; 
        int pB = players[i].current_point.row; 
        int pT = players[i].current_point.row + PLAYER_HEIGHT;

        // overlap check
        if(bR >= pL && bL <= pR && bT >= pB && bB <= pT){
            bball.is_held = true; 
            bball.held_by = i + 1; 
            bball.airborne = false; 

            if(i == 0){
                bball.current_point.col = pL;
            }
            else{
                bball.current_point.col = pL;
            }
            bball.current_point.row = PLAYER_HEIGHT >> 1; 

            return; 
        }
    }
    // no overlap means tha the ball is not held 
    bball.is_held = false; 
}
void throw_logic(void){
    // do x direction movement
    if(bball.held_by == 1){
        if(bball.current_point.col <= 120 ){
            bball.current_point.col++;
        }
        else if(bball.current_point.col > 120 && bball.current_point.col <= 150){
            bball.current_point.col += 2;
        }
        else if(bball.current_point.col > 150 && bball.current_point.col <= 170){
            bball.current_point.col += 5; 
        }
        else if(bball.current_point.col > 170 && bball.current_point.col <= 190){
            bball.current_point.col += 2;
        }
        else{
            bball.current_point.col++; 
        } 
    }
    else if(bball.held_by == 2){
        if(bball.current_point.col >= 120){
            bball.current_point.col--;
        }
        else if(bball.current_point.col < 120 && bball.current_point.col >= 110){
            bball.current_point.col -= 2; 
        }
        else if(bball.current_point.col < 110 && bball.current_point.col >= 90){
            bball.current_point.col -= 5; 
        }
        else if(bball.current_point.col < 90 && bball.current_point.col >= 70){
            bball.current_point.col -= 2; 
        }
        else{
            bball.current_point.col--;
        }
    }

    if(bball.ball_dir == UP){
        if(bball.current_point.row <= 50){
            bball.current_point.row += 30;
        }
        else if(bball.current_point.row > 50 && bball.current_point.row <= 150){
            bball.current_point.row += 20;
        }
        else if(bball.current_point.row > 150 && bball.current_point.row <= 190){
            bball.current_point.row += 10;
        }
        else if(bball.current_point.row > 190 && bball.current_point.row < bball.max_height){
            bball.current_point.row++;
        }
        else if(bball.current_point.row >= bball.max_height){
            bball.ball_dir = DOWN;
        }
    }
    else if(bball.ball_dir == DOWN){
        if(bball.current_point.row <= 10){
            bball.ball_dir = UP;
            bball.shoot_ball = false; 
        }
        else if(bball.current_point.row > 10 && bball.current_point.row <= 50){
            bball.current_point.row -= 30;
        }
        else if(bball.current_point.row > 50 && bball.current_point.row <= 150){
            bball.current_point.row -= 20;
        }
        else if(bball.current_point.row > 150 && bball.current_point.row <= 190){
            bball.current_point.row -= 10;
        }
        else if(bball.current_point.row > 190 && bball.current_point.row <= bball.max_height){
            bball.current_point.row--;
        }
    }
}
void bounce_ball(void){
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
}
void ball_movement(void){
    if(++slow < 10){
        return; 
    }
    slow = 0; 
    if(!bball.shoot_ball){
        bball.max_height = 50;      
    }
    else{
        bball.max_height = 200;
    }
    if(bball.max_height == 50){
        bounce_ball();
    }
    else{
        throw_logic();       
    }

    boundary_cond();
}
void check_ball_hoop(void){
    if(bball.current_point.col >= HOOP_WIDTH && bball.current_point.col <= HOOP_WIDTH + NET_LENGTH){
        if(bball.current_point.row <= (NET_HEIGHT + NET_WIDTH) && bball.current_point.row >= NET_HEIGHT){
            if(bball.vy < 0){
                hoops[0].prev_score = hoops[0].score;
                if(players[1].current_point.col > 60){
                    hoops[0].score += 2;
                }
                else if(players[1].current_point.col <= 60){
                    hoops[0].score++;
                }       
            }
            hoops[0].is_hit = true; 
            bball.airborne = false; 
            bball.vx = 0; 
            bball.vy = 0; 
        }    
    }
    
    if(bball.current_point.col >= X_MAX - HOOP_WIDTH - NET_LENGTH && bball.current_point.col <= X_MAX - HOOP_WIDTH){
        if(bball.current_point.row <= (NET_HEIGHT + NET_WIDTH) && bball.current_point.row >= NET_HEIGHT){
            if(bball.vy < 0){
                hoops[1].score = hoops[1].prev_score;
                if(players[0].current_point.col + PLAYER_WIDTH < 170){
                    hoops[1].score += 2;
                }
                else if(players[0].current_point.col + PLAYER_WIDTH >= 170){
                    hoops[1].score++;
                }   
            }
            hoops[1].is_hit = true;
            bball.airborne = false; 
            bball.vx = 0; 
            bball.vy = 0; 
        }    
    }
}
void reset_ball(void){
    draw_ball(BG_COLOR);
    bball.current_point.col = 115; 
    bball.current_point.row = 50; 
    bball.held_by = 0; 
    bball.is_held = false; 
    bball.shoot_ball = false; 
    bball.max_height = 50; 
    bball.prev_x = bball.current_point.col; 
    bball.prev_y = bball.current_point.row;
    bball.airborne = true; 
    draw_ball(BALL_COLOR);
}
void draw_scoreboard(void){
    G8RTOS_WaitSemaphore(&sem_I2CA);
    ST7789_DrawRectangle(0, 260-SCOREBOARD_HEIGHT, X_MAX, SCOREBOARD_HEIGHT, SCOREBOARD_COLOR);
    G8RTOS_SignalSemaphore(&sem_I2CA);
}
void check_win(void){
    if(hoops[0].score >= SCORE_WIN_COUNT || hoops[1].score >= SCORE_WIN_COUNT){
        game_over = true; 
    }
}
void update_score(void){
    if(hoops[0].score > hoops[0].prev_score){
        hoops[0].prev_score = hoops[0].score;
        G8RTOS_WaitSemaphore(&sem_SPIA);
        draw_scoreboard();
        ST7789_DrawStringStatic("MJ: ", BG_COLOR, 10, 240);
        ST7789_DrawStringStatic("LBJ: ", BG_COLOR, 140, 240);
        ST7789_DrawStringStatic(citoa(hoops[1].score, P1_BUFF, 10), BG_COLOR, 80, 240);
        ST7789_DrawStringStatic(citoa(hoops[0].score, P2_BUFF, 10), BG_COLOR, 210, 240);
        G8RTOS_SignalSemaphore(&sem_SPIA);      
    }
    if(hoops[1].score > hoops[1].prev_score){
        hoops[1].prev_score = hoops[1].score;
        G8RTOS_WaitSemaphore(&sem_SPIA);
        draw_scoreboard();
        ST7789_DrawStringStatic("MJ: ", BG_COLOR, 10, 240);
        ST7789_DrawStringStatic("LBJ: ", BG_COLOR, 140, 240);
        ST7789_DrawStringStatic(citoa(hoops[1].score, P1_BUFF, 10), BG_COLOR, 90, 240);
        ST7789_DrawStringStatic(citoa(hoops[0].score, P2_BUFF, 10), BG_COLOR, 220, 240);
        G8RTOS_SignalSemaphore(&sem_SPIA); 
    }
}
/*************************************Threads***************************************/
// Background Threads 
void Idle_Thread_BB(void) {
    for(;;){
    }
}
void Game_Init_BB(void){
    for(;;){
        if(start_screen){
            if(display_start){
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_Fill(ST7789_BLUE);
                ST7789_DrawStringStatic("WELCOME TO NBA 67K!", ST7789_WHITE, 20, 160);
                ST7789_DrawStringStatic("Press Joystick Button", ST7789_WHITE, 5, 120);
                ST7789_DrawStringStatic("to start the game!", ST7789_WHITE, 20, 100);
                G8RTOS_SignalSemaphore(&sem_SPIA);
                display_start = false; 
            }  
        }
        if(game_begin){
            ST7789_Fill(ST7789_BLACK);
            bball.current_point.col = 115;
            bball.current_point.row = 50;
            bball.max_height = 50; 
            bball.shoot_ball = false; 
            bball.ball_dir = DOWN;
            bball.airborne = true;
            bball.vy = 10; 

            hoops[0].score = 0;
            hoops[1].score = 0;
            hoops[0].prev_score = hoops[1].prev_score = hoops[0].score;

            reset_position(&players[0], 0);            
            reset_position(&players[1], 1);
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(0, 0, X_MAX, 10, GROUND_COLOR);
            ST7789_DrawRectangle(30, 0, 10, 10, ST7789_WHITE);
            ST7789_DrawRectangle(60, 0, 10, 10, ST7789_WHITE);
            ST7789_DrawRectangle(115, 0, 10, 10, ST7789_WHITE);
            ST7789_DrawRectangle(170, 0, 10, 10, ST7789_WHITE);
            ST7789_DrawRectangle(200, 0, 10, 10, ST7789_WHITE);
            G8RTOS_SignalSemaphore(&sem_SPIA);
            draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
            //draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);
            draw_player0_sprite();
            draw_hoop_sprite(&hoops[0], 0);
            draw_hoop_sprite(&hoops[1], 1);
            draw_ball_sprite();
            draw_scoreboard();
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawStringStatic("MJ: ", BG_COLOR, 10, 240);
            ST7789_DrawStringStatic("LBJ: ", BG_COLOR, 140, 240);
            ST7789_DrawStringStatic(citoa(hoops[0].score, P1_BUFF, 10), BG_COLOR, 80, 240);
            ST7789_DrawStringStatic(citoa(hoops[1].score, P2_BUFF, 10), BG_COLOR, 210, 240);
            G8RTOS_SignalSemaphore(&sem_SPIA);

            bball.prev_x = bball.current_point.col;
            bball.shoot_ball = false; 
            game_begin = false;
            

            // periodic threads break if they are child threads
            G8RTOS_AddThread(Update_Screen, 21, "UPDATE", 2);
            G8RTOS_AddThread(Read_Button, 22, "READBUTT", 3);
        }
        if(game_over){
            // kill children
            G8RTOS_KillThread(2);
            G8RTOS_KillThread(3);

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_Fill(ST7789_BLUE);
            G8RTOS_SignalSemaphore(&sem_SPIA);

            if(hoops[0].score > hoops[1].score){
                ST7789_DrawStringStatic("THE KING WON!", ST7789_WHITE, 10, 140);
            }
            else if(hoops[1].score > hoops[0].score){
                ST7789_DrawStringStatic("AIR JORDAN WON!", ST7789_WHITE, 10, 140);
            }
            else{
                ST7789_DrawStringStatic("RESTART?", ST7789_WHITE, 10, 140);
            }

            if(go_start){
                game_over = false; 
                start_screen = true; 
                display_start = true;
                go_start = false;
            }
        }
        sleep(10);
    }
}
void Update_Screen(void){
    for(;;){
        // bball.prev_x = bball.current_point.col;
        // bball.prev_y = bball.current_point.row; 

        // update_char(&players[1], del_x);
        
        // check_win();

        // if(players[0].is_moved){
        //     draw_player(&players[0], BG_COLOR, players[0].prev_x);
        //     draw_player(&players[0], PLAYER1_COLOR, players[0].current_point.col);
        //     // draw_player0_sprite();
        //     players[0].is_moved = false;
        // }
        // if(players[1].is_moved){
        //     draw_player(&players[1], BG_COLOR, players[1].prev_x);
        //     draw_player(&players[1], PLAYER2_COLOR, players[1].current_point.col);
        //     players[1].is_moved = false;
        // }

        // if(bball.airborne){
        //     draw_ball(BG_COLOR);
        //     physics_update();
        //     boundary_cond();
        //     check_ball_hoop();
        //     draw_ball_sprite();
        // }
        // else if(bball.is_held){
        //     draw_ball(BG_COLOR);
        //     pickup_ball();
        //     shoot_logic();
        //     boundary_cond();
        //     draw_ball_sprite();
        // }
        // else{
        //     pickup_ball();
        // }

        // if(hoops[0].is_hit){
        //     reset_ball();
        //     reset_players();
        //     draw_hoop_sprite(&hoops[0], 0);
        // }
        // else if(hoops[1].is_hit){
        //     reset_ball();
        //     reset_players();
        //     draw_hoop_sprite(&hoops[1], 1);
        // }

        // update_score();

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
        if(data_not & SW1){
            if(bball.is_held){
                bball.shoot_ball = true;  
                bball.ball_dir = UP;
            }
            G8RTOS_WaitSemaphore(&sem_UART);
            UARTprintf("BBall.SHOOT = %d\n\n", bball.shoot_ball);
            G8RTOS_SignalSemaphore(&sem_UART);
        }
        else if(data_not & SW2){
            // steal functionality
            if(bball.is_held){
                if(bball.held_by == 1){
                    bball.held_by = 2;
                }
                else if(bball.held_by == 2){
                    bball.held_by = 1;
                }

                bball.airborne = false; 

                
                if(bball.held_by == 1){
                    bball.current_point.col = players[0].current_point.col + PLAYER_WIDTH;
                }
                else if(bball.held_by == 2){
                    bball.current_point.col = players[1].current_point.col - BALL_RAD;
                }
                draw_ball(BG_COLOR);
                draw_ball(BALL_COLOR);
            }
        }
        else if(data_not & SW3){
            go_start = true;  // flag that will let me go back to start screen if I really want to
        }
        else if(data_not & SW4){
            bball.shoot_ball = false; 
            game_over = true; 
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
void Read_Joystick(void){
    for(;;){
        G8RTOS_WaitSemaphore(&sem_JOY);
        sleep(10);
        uint32_t data = GPIOPinRead(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
        if(data == 0){

            if(start_screen){
                start_screen = false; 
                game_begin = true;
            }
            // toggle joystick flag value
            if(game_over){
                game_begin = true;
                game_over = false;
            }
            
        }
        GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
        sleep(10);
    }

}

/********************** Periodic Threads *****************************/
void Move_Character(void){
    del_x = (int16_t)JOYSTICK_GetX();
}
void Move_Opp(void){
    int16_t opp_mov = 0;
    if(bball.held_by == 1){
        opp_mov = rand() % 2;
    }
    else if(bball.held_by == 2){
        if(players[0].current_point.col > players[1].current_point.col){
            opp_mov = rand() % 2 - 1;

            
        }
        if(players[0].current_point.col <= players[1].current_point.col)
        {
            opp_mov = rand() % 2; 
        }
    }
    else{
        opp_mov = rand() % 3 - 1;
    }
    update_opp(&players[0], opp_mov);
}
void Shoot_Opp(void){
    if(bball.held_by == 1 && bball.current_point.col > 120 && bball.is_held){
        bball.shoot_ball = rand() % 2; // randomly hold or shoot ball
        if(bball.shoot_ball){
            bball.is_held = false; 
            bball.ball_dir = UP;
        }
    } 
}

/********************** APeriodic Threads *****************************/
void Button_Handler(void){
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555);
}
void Joystick_Handler(void){
    GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
   	GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_JOY);
}
