
/************************************Includes***************************************/

#include "Quiz_Practice/breakout_threads.h"
#include "G8RTOS/G8RTOS_Scheduler.h"
#include "G8RTOS/G8RTOS_IPC.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define S_MAX 50
#define JOY_U_BOUND (2048+250)
#define JOY_L_BOUND (2048-250)
#define PIX_OFFSET 5
#define GRID_W 24
#define GRID_L 28
#define RECT_H 6
#define RECT_W 7
#define PIX_SQU 10
#define RECT_W_PIX 60
#define RECT_H_PIX 10
#define BOX_DIM_PIX 10
/*************************************Defines***************************************/

/*********************************Global Variables**********************************/
typedef enum dir{
    UP = 0, 
    DOWN = 1, 
    LEFT = 2, 
    RIGHT = 3, 
    NONE = 4
} dir;

typedef struct Point{
    int16_t row; 
    int16_t col;
} Point; 
 

typedef struct Block{
    Point current_point; 
    bool is_moving; 
    dir box_dir_x;
    dir box_dir_y;
    bool is_alive;
} Block;


/*********************************Global Variables**********************************/
// define global box entity based on block struct
static Block box;
static Block rect;
static bool game_begin = true;
static bool game_over = false;
static uint16_t joy_data_x;
// static uint16_t joy_data_y;
static Block rect_array[RECT_H][RECT_W];
static int16_t color_array[6] = {ST7789_BLUE, ST7789_GREEN, ST7789_RED, ST7789_ORANGE, ST7789_WHITE, ST7789_YELLOW};
static dir next_x;
static bool hit_block = false;

/*********************************** FUNCTIONS ********************************/

// Prototypes
void draw_block_bk(Block* block, uint16_t color);
void draw_segments(void);
void draw_rect_bottom(int16_t color);
void check_if_box(void);
static void move_box(void);
static void check_edge(void);
static void move_rect(void);
static void check_lose(void);
static void check_collision(void);
static void check_seg_coll(Block* block);
static inline dir opposite_dir(dir opp);




// Definitions
void draw_block_bk(Block* block, uint16_t color){
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle((*block).current_point.col, (*block).current_point.row, 10, 10, color);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

void draw_segments(){
    for(int8_t i = 0; i < RECT_H; ++i){
        for(int8_t j = 0; j < RECT_W; ++j){
            rect_array[i][j].current_point.row = 220 - 20*i + 15;
            rect_array[i][j].current_point.col = 160 - 20*j + 15;
            rect_array[i][j].is_moving = false; 
            rect_array[i][j].is_alive = true; 
            draw_block_bk(&rect_array[i][j], color_array[i]); // let each row be the same color
        }
    }
}

void draw_rect_bottom(int16_t color){
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle(rect.current_point.col, rect.current_point.row, RECT_W_PIX, RECT_H_PIX, color);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

void check_if_box(void){
    for(int8_t i = 0; i < RECT_H; ++i){
        for(int8_t j = 0; j < RECT_W; ++j){
            // pass
        }
    }
}

static void check_edge(void){
    // edge cases for the blocks
    if(box.current_point.col + 5 >= X_MAX){
        box.box_dir_x = LEFT;
    }
    else if(box.current_point.col - 5 <= 0){
        box.box_dir_x = RIGHT;
    }
    
    if(box.current_point.row + 5 >= Y_MAX){
        box.box_dir_y = DOWN;
    }
    else if(box.current_point.row - 5 <= 0){
        box.box_dir_y = UP;
    }
}

// static void check_lose(void){
//     // edge cases for the blocks
//     if(snake.snake_array[0].current_point.col + 5 >= X_MAX){
//         snake.snk_dir = NONE;
//         game_over = true;
//     }
//     else if(snake.snake_array[0].current_point.col - 5 <= 0){
//         snake.snk_dir = NONE;
//         game_over = true;
//     }
//     else if(snake.snake_array[0].current_point.row + 5 >= Y_MAX){
//         snake.snk_dir = NONE;
//         game_over = true;
//     }
//     else if(snake.snake_array[0].current_point.row - 5 <= 0){
//         snake.snk_dir = NONE;
//         game_over = true;
//     }

//     if(game_over){
//         snake.snake_array[0].current_point.row = 55;
//         snake.snake_array[0].current_point.col = 55; 
//     }
    

//     for(uint8_t i = 1; i < snake.snake_size; ++i){
//         if((snake.snake_array[snake.head_index].current_point.row == snake.snake_array[i].current_point.row) && (snake.snake_array[snake.head_index].current_point.col == snake.snake_array[i].current_point.col)){
//             game_over = true; 
//         }
//     }
// }

static void move_box(void){
    // check for box's direction
    if(box.is_moving){
        draw_block_bk(&box, ST7789_BLACK);

        if(box.box_dir_y == UP){
            box.current_point.row+=10;
        }
        else{
            box.current_point.row-=10;
        }

        if(box.box_dir_x == RIGHT){
            box.current_point.col+=10;
        }
        else{
            box.current_point.col-=10;
        }
    }
}

static void move_rect(void){

    // G8RTOS_WaitSemaphore(&sem_UART);
    // UARTprintf("x_val: %u, y_val: %u\n\n", rect.current_point.col, rect.current_point.row);
    // UARTprintf("x_val: %u, y_val: %u\n\n", box.current_point.col, box.current_point.row);
    // G8RTOS_SignalSemaphore(&sem_UART);

    
    draw_rect_bottom(ST7789_BLACK);

    rect.box_dir_x = next_x;
    if(rect.box_dir_x == RIGHT){
        rect.current_point.col += 10;
    }
    else if(rect.box_dir_x == LEFT){
        rect.current_point.col -= 10;
    }

    // boundary conditions for the rectangle's movement (defined by left corner)
    if(rect.current_point.col <= 0){
        rect.current_point.col = 0;
    }
    else if(rect.current_point.col + RECT_W_PIX >= X_MAX){
        rect.current_point.col = X_MAX - RECT_W_PIX;
    }

    draw_rect_bottom(ST7789_WHITE);
    
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle(0, 0, 60, 10, ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

void check_collision(void){
    
    // check for rectangle collision first
    if((box.current_point.row == rect.current_point.row + RECT_H_PIX) && (box.current_point.col >= rect.current_point.col && box.current_point.col <= (rect.current_point.col + RECT_W_PIX)) && (box.box_dir_y == DOWN)){
        box.box_dir_y = UP;
    }

    for(int8_t i = 0; i < RECT_H; ++i){
        for(int8_t j = 0; j < RECT_W; ++j){
            check_seg_coll(&rect_array[i][j]);
            if(hit_block){
                hit_block = false; 
                   }
        }
    }
}

static void check_seg_coll(Block* block){
    if((*block).is_alive){
            // Check top or bottom collision
        if (box.current_point.col >= (*block).current_point.col &&
            box.current_point.col < (*block).current_point.col + RECT_W_PIX &&
            box.current_point.row == (*block).current_point.row + RECT_H_PIX) {

            box.box_dir_y = DOWN;
            hit_block = true;
            draw_block_bk(block, ST7789_BLACK);
            (*block).is_alive = false; 

        }
        else if (box.current_point.col >= (*block).current_point.col &&
            box.current_point.col < (*block).current_point.col + RECT_W_PIX &&
            box.current_point.row + BOX_DIM_PIX == (*block).current_point.row) {
            
            box.box_dir_y = UP;
            hit_block = true;
            draw_block_bk(block, ST7789_BLACK);
            (*block).is_alive = false; 
        }

        // Check left or right collision
        else if (box.current_point.row >= (*block).current_point.row &&
            box.current_point.row < (*block).current_point.row + RECT_H_PIX &&
            box.current_point.col + BOX_DIM_PIX == (*block).current_point.col) {
            
            box.box_dir_x = LEFT;
            hit_block = true;
            draw_block_bk(block, ST7789_BLACK);
            (*block).is_alive = false; 
        }
        else if (box.current_point.row >= (*block).current_point.row &&
            box.current_point.row < (*block).current_point.row + RECT_H_PIX &&
            box.current_point.col == (*block).current_point.col + RECT_W_PIX) {

            box.box_dir_x = RIGHT;
            hit_block = true;
            draw_block_bk(block, ST7789_BLACK);
            (*block).is_alive = false; 
        }
    }  
}


/*************************************Threads***************************************/
// Working Threads 
void Idle_Thread_Breakout(void) {
    for(;;){
    }
}

void BK_Init(void){
    for(;;){
        if(game_begin){
            // draw the beeg segments
            draw_segments();

            rect.current_point.col = X_MAX / 2 - 30; 
            rect.current_point.row = 20;
            rect.is_moving = true; 
            rect.box_dir_x = NONE;
            
            box.current_point.col = X_MAX / 2 - 5;
            box.current_point.row = rect.current_point.row + RECT_H_PIX; // rect width
            box.is_moving = false; 
            box.box_dir_x = RIGHT;
            box.box_dir_y = UP;
            
            draw_rect_bottom(ST7789_WHITE);
            draw_block_bk(&box, ST7789_GREEN);
            game_begin = false;
        }

        if(game_over){
            draw_block_bk(&box, ST7789_BLUE);
        }

        // sleep after initialization thread
        sleep(10);
    }
}

void BK_Update(void){
    if(!game_over){
        check_edge();
        check_collision();
        move_box();
        move_rect();
        // check_lose();
        // draw_block(&snake.snake_array[snake.head_index], ST7789_WHITE);
        draw_block_bk(&box, ST7789_GREEN);
    }
}

// periodic thread
void Get_Joystick_BK(void) {

    joy_data_x = JOYSTICK_GetX();

    if(!game_over){
        dir proposed_x = rect.box_dir_x;
    
        if(joy_data_x <= JOY_L_BOUND){
            proposed_x = RIGHT;
        }
        else if(joy_data_x >= JOY_U_BOUND){
            proposed_x = LEFT;
        }
        else{
            proposed_x = NONE; 
        }

        next_x = proposed_x;
    }
}

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

void Restart_BK(void){
    for(;;){
        G8RTOS_WaitSemaphore(&sem_JOY);
        sleep(10);
        uint32_t data = GPIOPinRead(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
        if(data == 0){
            // toggle joystick flag value
            game_begin = true; 
            game_over = false; 
        }
        GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    }
}


void BK_GPIOD_Handler() {
    GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
   	GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_JOY);
}

void Box_Mov(void){
for(;;){
        G8RTOS_WaitSemaphore(&sem_PCA9555);        

        // sleep for a bit
        sleep(10);

        
        G8RTOS_WaitSemaphore(&sem_I2CA);
        uint8_t data = MultimodButtons_Get();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        

        uint8_t SW1P = 0;
        uint8_t SW2P = 0;
        uint8_t SW3P = 0;
        uint8_t SW4P = 0;

        if(~data & SW1){
            box.is_moving = true;
        }
        else if(~data & SW2){
            SW2P = 1;
        }
        else if(~data & SW3){
            SW3P = 1;
        }
        else if(~data & SW4){
            SW4P = 1;
        }
        

        // this helps prevent the pin from activating on a rising edge (weird issue I ran into)
        uint8_t released;
        do {
            G8RTOS_WaitSemaphore(&sem_I2CA);
            released = MultimodButtons_Get();
            G8RTOS_SignalSemaphore(&sem_I2CA);
            sleep(1);
        } while (~released & (SW1 | SW2 | SW3 | SW4));

        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    }
}

void BK_GPIOE_Handler(void){
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555);
}
