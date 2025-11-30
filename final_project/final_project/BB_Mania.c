// Lab 3, uP2 Fall 2025
// Created: 2023-07-31
// Updated: 2025-07-09
// Lab 3 is intended to introduce you to RTOS concepts. In this, you will
// - configure the systick function
// - write asm functions for context switching
// - write semaphore functions
// - write scheduler functions to add threads / run scheduling algorithms
// - write critical section assembly functions

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./Quiz_Practice/bb_mania_threads.h"
#include <driverlib/fpu.h>
#include "time.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/
//extern uint32_t SystemTime;

/********************************Public Variables***********************************/
/********************************Public Functions***********************************/


/********************************Public Functions***********************************/

/************************************MAIN*******************************************/

// Be sure to add in your source files from previous labs into "MultimodDrivers/src/"!
// If you made any modifications to the corresponding header files, be sure to update
// those, too.
int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    Multimod_Init();
    ST7789_Fill(ST7789_BLACK);

    G8RTOS_InitSemaphore(&sem_UART, UART_Resources);
    G8RTOS_InitSemaphore(&sem_I2CA, I2C_Resources);
    G8RTOS_InitSemaphore(&sem_SPIA, SPI_Resources);
    G8RTOS_InitSemaphore(&sem_PCA9555, MMB_Resources);
    G8RTOS_InitSemaphore(&sem_JOY, JOY_Resources);
    G8RTOS_InitSemaphore(&sem_KillCube, KillCube_Resources);

    G8RTOS_Init();
    G8RTOS_InitFIFO(MOVEMENT_FIFO);

    // IDLE THREAD
    G8RTOS_AddThread(Idle_Thread_BB, MIN_PRIORITY, "IDLE", 200);

    // APERIODIC THREADS
    // G8RTOS_Add_APeriodicEvent(BK_GPIOD_Handler, 2, INT_GPIOD);
    G8RTOS_Add_APeriodicEvent(Button_Handler, 3, INT_GPIOE);

    // Background Threads
    G8RTOS_AddThread(Game_Init_BB, 20, "START", 1);
    G8RTOS_AddThread(Update_Screen, 1, "UPDATE", 3);
    //G8RTOS_AddThread(Read_Button, 23, "READBUTT", 4);

    // PERIODIC THREADS
    G8RTOS_Add_PeriodicEvent(Move_Character, 100, 5);
    G8RTOS_Add_PeriodicEvent(Move_Opp, 200, 7);


    
    G8RTOS_Launch();

    for(;;){}
}

/************************************MAIN*******************************************/
