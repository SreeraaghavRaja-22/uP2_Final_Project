#ifndef FROGGER_THREADS_H_
#define FROGGER_THREADS_H_

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
void Idle_Thread_Frogger(void);
void Frogger_Init(void);
void Move_Frog(void);
void Update_Cars(void);
void Get_Joystick_Frogger(void);
void Game_Over_Frogger(void);
void Restart_Frogger(void);
void Frogger_GPIOE_Handler(void);
void Frogger_GPIOD_Handler(void);



#endif /* FROGGER_THREADS_H */
