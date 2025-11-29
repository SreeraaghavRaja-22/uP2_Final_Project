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

#include "./threads.h"

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
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initializes the necessary peripherals.
    Multimod_Init();

    ST7789_Fill(ST7789_BLUE);
   
    // Add threads, initialize semaphores here!
    G8RTOS_InitSemaphore(&sem_UART, UART_Resources);
    G8RTOS_InitSemaphore(&sem_I2CA, I2C_Resources);
    G8RTOS_InitSemaphore(&sem_SPI, SPI_Resources);
    G8RTOS_InitSemaphore(&sem_SW1, SW_Resources);

    // initialize the FIFOs
    G8RTOS_InitFIFO(0);
    G8RTOS_Init();
    //G8RTOS_Add_APeriodicEvent(SW1_ISR, 2, INT_GPIOF);
    //G8RTOS_Add_APeriodicEvent(SW2_ISR, 2, INT_GPIOF);
    //G8RTOS_AddThread(Accel, 0, "Accel", 32);
    //G8RTOS_AddThread(Gyro, 1, "Gyro", 64);

    // G8RTOS_AddThread(Opto, 2, "Opto", 96);
    G8RTOS_AddThread(FIFOProducer, 3, "FIFPROD", 100);
    G8RTOS_AddThread(FIFOConsumer, 5, "FIFCONS", 101);
    //G8RTOS_AddThread(LCDThread, 5, "LCDThread", 102);
    //G8RTOS_AddThread(SW1_Event_Handler, 5, "SW1E", 20);
    //G8RTOS_AddThread(SW2_Event_Handler, 1, "SW2E", 343);
    G8RTOS_Add_PeriodicEvent(Idle_Thread_Periodic, 300, 300);
    //G8RTOS_Add_PeriodicEvent(PThread2, 310, 300); // same period but staggered by 1 ms
    G8RTOS_AddThread(Idle_Thread, MIN_PRIORITY, "IDLE", 200);
    G8RTOS_Launch();

    // spin - the RTOS will take over now
    while(1);
}

/************************************MAIN*******************************************/
