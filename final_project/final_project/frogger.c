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

#include "./Quiz_Practice/frogger_threads.h"
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

    // sysclock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initializes the necessary peripherals.
    Multimod_Init();

    ST7789_Fill(ST7789_BLACK);
   
    // Add threads, initialize semaphores here!
    G8RTOS_InitSemaphore(&sem_UART, UART_Resources);
    G8RTOS_InitSemaphore(&sem_I2CA, I2C_Resources);
    G8RTOS_InitSemaphore(&sem_SPI, SPI_Resources);
    G8RTOS_InitSemaphore(&sem_PCA9555, MMB_Resources);
    G8RTOS_InitSemaphore(&sem_JOY, JOY_Resources);
    G8RTOS_InitSemaphore(&sem_KillCube, KillCube_Resources);

    G8RTOS_Init();

    // IDLE THREAD
    G8RTOS_AddThread(Idle_Thread_Frogger, MIN_PRIORITY, "IDLE", 200);

    // APERIODIC THREADS
    // G8RTOS_Add_APeriodicEvent(BK_GPIOD_Handler, 2, INT_GPIOD);
    // G8RTOS_Add_APeriodicEvent(BK_GPIOE_Handler, 3, INT_GPIOE);

    // Background Threads
    G8RTOS_AddThread(Frogger_Init, 20, "GAME_INIT", 1);
    //G8RTOS_AddThread(Update_Cars, 21, "Move_Cars", 3);
    //G8RTOS_AddThread(Restart_Frogger, 1, "RESTART", 88);

    // PERIODIC THREADS
    G8RTOS_Add_PeriodicEvent(Move_Frog, 175, 6); // same period but staggered by 1 ms
    G8RTOS_Add_PeriodicEvent(Get_Joystick_Frogger, 30, 9);


 
    
    G8RTOS_Launch();

    for(;;){}
}

/************************************MAIN*******************************************/
