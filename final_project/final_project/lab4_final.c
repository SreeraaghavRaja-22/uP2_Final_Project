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
#include <driverlib/fpu.h>
#include "time.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define JOYSTICK_FIFO 0
#define SPAWNCOOR_FIFO 1
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


    //fix floating point issues
    float num = 1.0 / 2.0; 
    FPUStackingDisable();

    
    // get a random seed 
    srand(time(120));

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

    // initialize the FIFOs
    G8RTOS_InitFIFO(JOYSTICK_FIFO);
    G8RTOS_InitFIFO(SPAWNCOOR_FIFO);
    G8RTOS_Init();

    // IDLE THREAD
    G8RTOS_AddThread(Idle_Thread, MIN_PRIORITY, "IDLE", 200);

    // APERIODIC THREAD
    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 4, INT_GPIOE);
    G8RTOS_Add_APeriodicEvent(GPIOD_Handler, 2, INT_GPIOD);

    // Background Threads
    G8RTOS_AddThread(Read_Buttons, 2, "Odysseus", 24);
    G8RTOS_AddThread(Read_JoystickPress, 1, "Telemachus", 22);
    G8RTOS_AddThread(CamMove_Thread, 3, "Penelope", 88);

    // PERIODIC THREADS
    G8RTOS_Add_PeriodicEvent(Print_WorldCoords, 100, 6);
    G8RTOS_Add_PeriodicEvent(Get_Joystick, 100, 7); // same period but staggered by 1 ms
    
    G8RTOS_Launch();

    // spin - the RTOS will take over now
    while(1);
}

/************************************MAIN*******************************************/
