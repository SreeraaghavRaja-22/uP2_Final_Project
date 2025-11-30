// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef BB_MANIA_THREADS_H_
#define BB_MANIA_THREADS_H_

/************************************Includes***************************************/

#include "../G8RTOS/G8RTOS_Semaphores.h"
#include "../G8RTOS/G8RTOS_CriticalSection.h"

/************************************Includes***************************************/
#define MOVEMENT_FIFO 0
/***********************************Semaphores**************************************/
semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_SW1;
semaphore_t sem_JOY;
semaphore_t sem_PCA9555;
semaphore_t sem_KillCube;

/***********************************Semaphores**************************************/

// thread definitions
void Idle_Thread_BB(void);
void Game_Init_BB(void);
void Update_Screen(void);
void Move_Character(void);
void Move_Opp(void);
void Button_Handler(void);



#endif /* SNAKE_THREADS_H */
