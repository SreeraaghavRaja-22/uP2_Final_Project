#ifndef BREAKOUT_THREADS_H_
#define BREAKOUT_THREADS_H_

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS_Semaphores.h"
#include "G8RTOS/G8RTOS_CriticalSection.h"

/************************************Includes***************************************/

/***********************************Semaphores**************************************/
semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPI;
semaphore_t sem_SW1;
semaphore_t sem_JOY;
semaphore_t sem_PCA9555;
semaphore_t sem_KillCube;
/***********************************Semaphores**************************************/

// thread definitions
void Idle_Thread_Breakout(void);
void BK_Init(void);
void BK_Update(void);
void Get_Joystick_BK(void);
void Game_Over_BK(void);
void Restart_BK(void);
void Box_Mov(void);
void BK_GPIOE_Handler(void);
void BK_GPIOD_Handler(void);



#endif /* BREAKOUT_THREADS_H */
