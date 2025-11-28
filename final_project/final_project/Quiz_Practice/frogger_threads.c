
/************************************Includes***************************************/

#include "./Quiz_Practice/frogger_threads.h"
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
#define SAFE_H 20
#define SAFE_W 240
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
    bool is_moved; 
    dir box_dir_x;
    dir box_dir_y;
    bool is_alive;
} Block;

typedef struct Car{
    Point current_point; 
    bool is_moving; 
    dir car_dir_x; 
    uint8_t speed; 
    bool delete;
} Car; 


/*********************************Global Variables**********************************/
// define global box entity based on block struct
static Block frog;
static Car car_array[4][4];
static Block safe[2];
static bool game_begin = true;
static bool game_over = false;
static uint16_t joy_data_x;
static uint16_t joy_data_y;
static int16_t color_array[6] = {ST7789_BLUE, ST7789_GREEN, ST7789_RED, ST7789_ORANGE, ST7789_WHITE, ST7789_YELLOW};
static dir next_x;
static dir next_y;
static dir prev_x;
static dir prev_y;
static int16_t prev_x_coor; 
static int16_t prev_y_coor; 
static bool hit_block = false;

/*********************************** FUNCTIONS ********************************/

// Prototypes
static void spawn_frog_coor();
static void draw_block_frogger(Block* block, uint16_t color, uint16_t bg_color);
static void draw_segments(Block* block);
static void update_frog_coor(void);
static void draw_car(Car* car, int16_t color);
static void update_car_pos(Car * car);
static void render_cars(void);
//static void check_edge(void);
//static void move_rect(void);
//static void check_lose(void);
//static void check_collision(void);
//static void check_seg_coll(Block* block);
//static inline dir opposite_dir(dir opp);


// Definitions
static void spawn_frog_coor(){
    frog.current_point.row = (rand() % (10 - 0 + 1 + 0)) + 20;
    frog.current_point.col = (rand() % ((X_MAX-20) - 20 + 1 + 20));

    if(frog.current_point.col <= 0){
        frog.current_point.col = 0;
    }
    else if(frog.current_point.col >= X_MAX){
        frog.current_point.col = X_MAX;
    }
}

static void draw_block_frogger(Block* block, uint16_t color, uint16_t bg_color){
    if(frog.is_moved){
        G8RTOS_WaitSemaphore(&sem_SPI);
        ST7789_DrawRectangle(prev_x_coor, prev_y_coor, 10, 10, bg_color);
        ST7789_DrawRectangle(frog.current_point.col, frog.current_point.row, 10, 10, color);
        G8RTOS_SignalSemaphore(&sem_SPI);
    }
    else{
        G8RTOS_WaitSemaphore(&sem_SPI);
        ST7789_DrawRectangle((*block).current_point.col, (*block).current_point.row, 10, 10, color);
        G8RTOS_SignalSemaphore(&sem_SPI);
    }  
}

static void draw_segments(Block *block){
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle((*block).current_point.col, (*block).current_point.row, SAFE_W, SAFE_H, ST7789_WHITE);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

static void update_frog_coor(){
    
    frog.box_dir_x = next_x; 
    frog.box_dir_y = next_y;
    prev_x_coor = frog.current_point.col; 
    prev_y_coor = frog.current_point.row;

    if(frog.box_dir_x == RIGHT){
    frog.current_point.col += 10;
    }
    else if(frog.box_dir_x == LEFT){
        frog.current_point.col -= 10;
    }

    if(frog.box_dir_y == UP){
        frog.current_point.row += 10;
    }
    else if(frog.box_dir_y == DOWN){
        frog.current_point.row -= 10;
    }

    if(frog.current_point.row >= 130){
        frog.current_point.row = 130;
    }
    else if(frog.current_point.row <= 20){
        frog.current_point.row = 20;
    }

     if(frog.current_point.col >= X_MAX){
        frog.current_point.col = X_MAX;
    }
    else if(frog.current_point.col <= 0){
        frog.current_point.col = 0;
    }
}

static void draw_car(Car* car, int16_t color){
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle((*car).current_point.col, (*car).current_point.row, 20, 10, color);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

static void update_car_pos(Car* car){
    if((*car).car_dir_x == RIGHT){
        (*car).current_point.col += (*car).speed;
    }
    else if((*car).car_dir_x == LEFT){
        (*car).current_point.col += (*car).speed;
    }

    if((*car).current_point.col >= X_MAX){
        (*car).delete = true;
        (*car).current_point.col = X_MAX;
    }
    else if((*car).current_point.col <= 0){
        (*car).current_point.col = 0;
    }
}


static void render_cars(void){
    for(int8_t i = 0; i < 4; ++i){
        for(int8_t j = 0; j < 4; ++j){
            car_array[i][j].car_dir_x = (rand() % 2) + 2;
            car_array[i][j].is_moving = true; 
            car_array[i][j].current_point.row = 40 + 20*i;
            car_array[i][j].current_point.col = (rand() % X_MAX);
            car_array[i][j].speed = rand() % 3 * 10; 
            draw_car(&car_array[i][j], ST7789_RED);
        }
    }
}


/*************************************Threads***************************************/
// Working Threads 
void Idle_Thread_Frogger(void) {
    for(;;){
    }
}

void Frogger_Init(void){
    for(;;){
        if(game_begin){
            // draw the safe zones 
            safe[0].current_point.row = 20;
            safe[0].current_point.col = 0;

            safe[1].current_point.row = 120; 
            safe[1].current_point.col = 0;

            // safe[1].current_point
            draw_segments(&safe[0]);
            draw_segments(&safe[1]);

            spawn_frog_coor();
            frog.box_dir_x = NONE;
            frog.box_dir_y = NONE;
            draw_block_frogger(&frog, ST7789_GREEN, ST7789_WHITE);

            render_cars();          
            game_begin = false;
        }

        if(game_over){
            //
        }

        // sleep after initialization thread
        sleep(10);
    }
}

// Peridic thread to move frog
void Move_Frog(void){
    if(frog.is_moved){
        update_frog_coor();
        int16_t bg_color = ST7789_WHITE; 
        if(frog.current_point.row > 40 && frog.current_point.row < 125){
            bg_color = ST7789_BLACK;
        }
        else{
            bg_color = ST7789_WHITE;
        }
        draw_block_frogger(&frog, ST7789_GREEN, bg_color);
        frog.is_moved = false;
    }
}

void Move_Car(void){
}
// void Frogger_Update(void){
//     if(!game_over){
//         check_edge();
//         check_collision();
//         move_box();
//         move_rect();
//         // check_lose();
//         // draw_block(&snake.snake_array[snake.head_index], ST7789_WHITE);
//     }
// }

// periodic thread
void Get_Joystick_Frogger(void) {

    joy_data_x = JOYSTICK_GetX();
    joy_data_y = JOYSTICK_GetY();

    prev_x = frog.box_dir_x; 
    prev_y = frog.box_dir_y;

    if(!game_over){
        dir proposed_x = frog.box_dir_x;
        dir proposed_y = frog.box_dir_y;
    
        if(joy_data_x <= JOY_L_BOUND){
            proposed_x = RIGHT;
        }
        else if(joy_data_x >= JOY_U_BOUND){
            proposed_x = LEFT;
        }
        else{
            proposed_x = NONE; 
        }

        if(joy_data_y <= JOY_L_BOUND){
            proposed_y = DOWN;
        }
        else if(joy_data_y >= JOY_U_BOUND){
            proposed_y = UP;
        }
        else{
            proposed_y = NONE; 
        }


        if(prev_x != proposed_x){
            next_x = proposed_x;
            frog.is_moved = true; 
        }
        else if(prev_y != proposed_y){
            next_y = proposed_y;
            frog.is_moved = true; 
        }
        else{
            next_x = prev_x;
            next_y = prev_y;
        }

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("Frog X: %u, Frog Y: %u\n\n", frog.box_dir_x, frog.box_dir_y);
        G8RTOS_SignalSemaphore(&sem_UART);
    }
}



void Update_Cars(void){
    for(int8_t i = 0; i < 2; ++i){
        for(int8_t j = 0; j < 2; ++j){
            draw_car(&car_array[i][j], ST7789_BLACK);
            update_car_pos(&car_array[i][j]);
            draw_car(&car_array[i][j], ST7789_RED);
        }
    }
}


// void Restart_Frogger(void){
//     for(;;){
//         G8RTOS_WaitSemaphore(&sem_JOY);
//         sleep(10);
//         uint32_t data = GPIOPinRead(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//         if(data == 0){
//             // toggle joystick flag value
//             game_begin = true; 
//             game_over = false; 
//         }
//         GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
//     }
// }


void Frogger_GPIOD_Handler() {
    GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
   	GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_JOY);
}

void Frog_Mov(void){
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
            frog.is_moved = true;
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

void Frogger_GPIOE_Handler(void){
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555);
}
