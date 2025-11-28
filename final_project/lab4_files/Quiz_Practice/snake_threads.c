
/************************************Includes***************************************/

#include "./Quiz_Practice/snake_threads.h"
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
#define PIX_SQU 10
/*************************************Defines***************************************/

/*********************************Global Variables**********************************/
// define location of snake with enum
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

// struct for the point a snake is at
typedef struct Point{
    int16_t row; 
    int16_t col;
} Point; 
 

typedef struct Block{

    Point current_point;
    dir snk_dir; 

} Block;

typedef struct Snake{
    Block snake_array[S_MAX];
    uint8_t head_index;
    uint8_t tail_index;
    dir snk_dir; 
    uint8_t snake_size;

} Snake;


/*********************************Global Variables**********************************/
// define global square entity based on block struct
static Block square;
static Block orange;
Snake snake;
static bool game_begin = true;
static bool game_over = false;
static uint16_t joy_data_x;
static uint16_t joy_data_y;
loc game_array[X_MAX][Y_MAX];
static dir next_dir;

/*********************************** FUNCTIONS ********************************/

// Prototypes
void draw_block(Block* block, uint16_t color);
void birth_snake(uint8_t snake_size);
void update_snake(void);
void snake_bigger(void);
void spawn_orange(void);
static void check_edge(void);
static void move_snake(void);
static void check_lose(void);
void check_collision(void);
static inline dir opposite_dir(dir opp);




// Definitions
void draw_block(Block* block, uint16_t color){
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle((*block).current_point.col, (*block).current_point.row, 10, 10, color);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

void birth_snake(uint8_t snake_size){
    for(uint8_t i = 0; i < snake_size; ++i){
        // compensate for pixel size and it's going left 
        snake.snake_array[i].current_point.col = 55 - 10*i;
        snake.snake_array[i].current_point.row = 55;
       
        if(i != 0){
            snake.snake_array[i].snk_dir = snake.snake_array[i-1].snk_dir;
        }
        draw_block(&snake.snake_array[i], ST7789_WHITE);
    }

    next_dir = snake.snk_dir;
}

void update_snake(void){
    snake.snk_dir = next_dir;
    // go from tail to head when updating
    for(int8_t i = snake.snake_size-1; i >= 1; --i){
        snake.snake_array[i] = snake.snake_array[i-1];
    }
}


void snake_bigger(void){
    // increment the tail index and snake size
    snake.snake_size++;
    snake.tail_index++;
    // set the new tail equal to the old tail
    snake.snake_array[snake.tail_index] = snake.snake_array[snake.tail_index-1];

    // update the coordinates of the new tail based on the previous tail's direction
    if(snake.snake_array[snake.tail_index-1].snk_dir == RIGHT){
        snake.snake_array[snake.tail_index].current_point.col -= 10;
    }
    else if(snake.snake_array[snake.tail_index-1].snk_dir == LEFT){
        snake.snake_array[snake.tail_index].current_point.col += 10;
    }
    else if(snake.snake_array[snake.tail_index-1].snk_dir == UP){
        snake.snake_array[snake.tail_index].current_point.row += 10;
    }
    else if(snake.snake_array[snake.tail_index-1].snk_dir == DOWN){
        snake.snake_array[snake.tail_index].current_point.row -= 10;
    }

    // draw the tail again
    draw_block(&snake.snake_array[snake.tail_index], ST7789_WHITE);
}

void spawn_orange(void){
        orange.current_point.col = (rand() % ((X_MAX / PIX_SQU) - 1) * PIX_SQU) + 5;
        orange.current_point.row = (rand() % ((Y_MAX / PIX_SQU) - 1) * PIX_SQU) + 5;
        draw_block(&orange, ST7789_ORANGE);
}


static void check_edge(void){
    // edge cases for the blocks
    if(square.current_point.col + 5 == X_MAX){
        square.current_point.col = 5;
    }
    else if(square.current_point.col - 5 == 0){
        square.current_point.col = X_MAX - 5;
    }
    else if(square.current_point.row + 5 == Y_MAX){
        square.current_point.row = 5;
    }
    else if(square.current_point.row - 5 == 0){
        square.current_point.row = Y_MAX-5;
    }
}

static void check_lose(void){
    // edge cases for the blocks
    if(snake.snake_array[0].current_point.col + 5 >= X_MAX){
        snake.snk_dir = NONE;
        game_over = true;
    }
    else if(snake.snake_array[0].current_point.col - 5 <= 0){
        snake.snk_dir = NONE;
        game_over = true;
    }
    else if(snake.snake_array[0].current_point.row + 5 >= Y_MAX){
        snake.snk_dir = NONE;
        game_over = true;
    }
    else if(snake.snake_array[0].current_point.row - 5 <= 0){
        snake.snk_dir = NONE;
        game_over = true;
    }

    if(game_over){
        snake.snake_array[0].current_point.row = 55;
        snake.snake_array[0].current_point.col = 55; 
    }
    

    for(uint8_t i = 1; i < snake.snake_size; ++i){
        if((snake.snake_array[snake.head_index].current_point.row == snake.snake_array[i].current_point.row) && (snake.snake_array[snake.head_index].current_point.col == snake.snake_array[i].current_point.col)){
            game_over = true; 
        }
    }
}

static void move_snake(void){
    // check for square's direction
    if(snake.snk_dir == UP){
        snake.snake_array[0].current_point.row += 10;
    }
    else if(snake.snk_dir == DOWN){
        snake.snake_array[0].current_point.row -= 10;
    }
    else if(snake.snk_dir == LEFT){
        snake.snake_array[0].current_point.col -= 10;
    }
    else if(snake.snk_dir == RIGHT){
        snake.snake_array[0].current_point.col += 10;
    }
}

void check_collision(void){
    if(snake.snake_array[snake.head_index].current_point.col == orange.current_point.col && snake.snake_array[snake.head_index].current_point.row == orange.current_point.row){
        draw_block(&orange, ST7789_BLACK);
        spawn_orange();
        snake_bigger();
    }
}

static inline dir opposite_dir(dir opp){
    switch(opp){
        case RIGHT : return LEFT; 
        case LEFT  : return RIGHT; 
        case UP    : return DOWN; 
        case DOWN  : return UP;
        default    : return NONE;
    }
}

/*************************************Threads***************************************/
// Working Threads 
void Idle_Thread_Snake(void) {
    for(;;){
    }
}

void Game_Init(void){
    for(;;){
        if(game_begin){
            // "square.current_point.row = 55; 
            // square.current_point.col = 55; 
            // square.snk_dir = RIGHT;"

            snake.snake_size = 3;
            snake.head_index = 0; 
            snake.tail_index = 2; 
            snake.snake_array[0].snk_dir = snake.snk_dir = RIGHT;
            

            // initialize the snake with 3 segments...exactly like the online game
            birth_snake(snake.snake_size);

            spawn_orange();
            
            // sleep just in case
            sleep(10);

            game_begin = false;
        }

        if(game_over){
            G8RTOS_AddThread(Game_Over, 1, "ENDGAME", 22);
        }
    }
}

void Game_Update(void){
    if(!game_over){
        draw_block(&snake.snake_array[snake.tail_index], ST7789_BLACK);
        update_snake();
        move_snake();
        check_lose();
        sleep(10);
        check_collision();
        draw_block(&snake.snake_array[snake.head_index], ST7789_WHITE);
    }
}

// periodic thread
void Get_Joystick_Snake(void) {

    joy_data_x = JOYSTICK_GetX();
    joy_data_y = JOYSTICK_GetY();
    if(!game_over){
        dir proposed = snake.snk_dir;
    
        if(joy_data_x <= JOY_L_BOUND){
            proposed = RIGHT;
        }
        else if(joy_data_x >= JOY_U_BOUND){
            proposed = LEFT;
        }
        else if(joy_data_y <= JOY_L_BOUND){
            proposed = DOWN;
        }
        else if(joy_data_y >= JOY_U_BOUND){
            proposed = UP;
        }

        if(proposed != opposite_dir(snake.snk_dir)){
            next_dir = proposed;
        }
    }
 
}

void Game_Over(void){
    for(;;){
        G8RTOS_WaitSemaphore(&sem_SPI);
        ST7789_DrawRectangle(orange.current_point.col, orange.current_point.row, 10, 10, ST7789_BLACK);
        for(int8_t i = 0; i < snake.snake_size; ++i){
            ST7789_DrawRectangle(snake.snake_array[i].current_point.col, snake.snake_array[i].current_point.row, 10, 10, ST7789_BLACK);
        }
        G8RTOS_SignalSemaphore(&sem_SPI);
        G8RTOS_KillSelf();
    }
}

void Restart_Game(void){
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

void Snake_GPIOD_Handler(void) {
    GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
   	GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_JOY);
}

