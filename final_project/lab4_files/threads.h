// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS_Semaphores.h"
#include "G8RTOS/G8RTOS_CriticalSection.h"

/************************************Includes***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPI;
semaphore_t sem_SW1;
semaphore_t sem_PCA9555;
semaphore_t sem_JOY;
semaphore_t sem_KillCube;



/***********************************Semaphores**************************************/


/********************************Thread Functions***********************************/

// Threads we'll use for this lab
void Idle_Thread(void);
void CamMove_Thread(void);
void Cube_Thread(void);
void Read_Buttons(void);
void Read_JoystickPress(void);
void Print_WorldCoords(void);
void Get_Joystick(void);
void GPIOE_Handler(void);
void GPIOD_Handler(void);

// Testing Threads / ISRS
void Accel(void);
void Gyro(void);
void Opto(void);
void FIFOProducer(void);
void FIFOConsumer(void);
void FIFOConsumer2(void);
void SW1_ISR(void);
void SW2_ISR(void);
void SW1_Event_Handler(void); 
void SW2_Event_Handler(void);
void ChildA_Thread(void); 
void ChildB_Thread(void);
void PThread1(void);
void PThread2(void);
void LCDThread(void);


/********************************Thread Functions***********************************/


#endif /* THREADS_H_ */

