// G8RTOS_Semaphores.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for semaphore functions


#include "../G8RTOS_Semaphores.h"

/************************************Includes***************************************/
#include <inc/hw_memmap.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "../G8RTOS_CriticalSection.h"
#include "../G8RTOS_Scheduler.h"


/************************************Includes***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/

int32_t IBit_State;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_InitSemaphore
// Initializes semaphore to a value. This is a critical section!
// Param "s": Pointer to semaphore
// Param "value": Value to initialize semaphore to
// Return: void
void G8RTOS_InitSemaphore(semaphore_t* s, int32_t value) {
   // your stuff goes here
   (*s).count = value;
}

// G8RTOS_WaitSemaphore
// Create a blocked semaphore
void G8RTOS_WaitSemaphore(semaphore_t* s) {
    IBit_State = StartCriticalSection(); 
    // decrement value of semaphore by 1
    (*s).count--;

    /* if the semaphore is less than 0, that means the resource is being used
    another thread is waiting on it too 
    */
    if((*s).count < 0)
    {
        // give the blocked field the reason why the semaphore is blocked
        CurrentlyRunningThread -> blocked = s;
        EndCriticalSection(IBit_State);

        // figure out how to implement this
        // trigger systick interrupt to enable thread switch
        // G8RTOS_Suspend();
        // HWREG(NVIC_ST_CURRENT) |= 0; // reset counter 

        // do this for a context switch (set SysTick interrupt)
        // HWREG(NVIC_ST_CTRL) |= NVIC_INT_CTRL_PENDSTSET;
        // asm("nop");

        // context switch
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
        return;
    }
    // enable interrupts
    EndCriticalSection(IBit_State);
}

// G8RTOS_SignalSemaphore
// Signals that the semaphore has been released by incrementing the value by 1.
// This is a critical section!
// Goes through a linked list of all TCBs and wakes up the first one that has a blocked field equal to the semaphore
// Wakes it up by setting the blocked field to 0
void G8RTOS_SignalSemaphore(semaphore_t* s) {
    IBit_State = StartCriticalSection();
    (*s).count++;
    // check for all the blocked threads (all were at most -1)
    if((*s).count <= 0)
    {
        // set up a pointer to the value of the next TCB
        tcb_t *pt = CurrentlyRunningThread -> nextTCB;
        // check for the first thread that is blocked by the same semaphore
        while(pt->blocked != s)
        {
            pt = pt -> nextTCB;
        }
        // wake up thread with matching semaphore blocking it
        pt -> blocked = 0;
    }
    EndCriticalSection(IBit_State);
}

// G8RTOS_Suspend
// Will halt the current thread if it is blocked and let another thread function

/********************************Public Functions***********************************/
