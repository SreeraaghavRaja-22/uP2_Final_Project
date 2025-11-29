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
void Game_Update_BB(void);
void Get_Joystick_BB(void);
void Game_Over_BB(void);
void Restart_Game_BB(void);
void BB_GPIOD_Handler(void);
void Idle_Thread_Periodic_BB(void);



#endif /* SNAKE_THREADS_H */
