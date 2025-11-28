// G8RTOS_Semaphores.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Semaphores

#ifndef G8RTOS_SEMAPHORES_H_
#define G8RTOS_SEMAPHORES_H_

/************************************Includes***************************************/

#include <stdint.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define UART_Resources 1
#define I2C_Resources 1
#define SPI_Resources 1
#define SW_Resources 0
#define MMB_Resources 0 
#define JOY_Resources 0
#define KillCube_Resources 1
/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/

// Semaphore typedef
// your stuff goes here
typedef struct semaphore_t{
    int32_t count; 
}semaphore_t;

// Semaphore instantiation
// your stuff goes here

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/


/********************************Public Functions***********************************/

void G8RTOS_InitSemaphore(semaphore_t* s, int32_t value);
void G8RTOS_WaitSemaphore(semaphore_t* s);
void G8RTOS_SignalSemaphore(semaphore_t* s);

/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* G8RTOS_SEMAPHORES_H_ */

