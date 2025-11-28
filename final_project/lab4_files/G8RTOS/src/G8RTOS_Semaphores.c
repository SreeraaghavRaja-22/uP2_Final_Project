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
   (*s) = value;
}

// G8RTOS_WaitSemaphore
// Create a blocked semaphore
void G8RTOS_WaitSemaphore(semaphore_t* s) {
    IBit_State = StartCriticalSection(); 
    // decrement value of semaphore by 1
    *(s)--;

    /* if the semaphore is less than 0, that means the resource is being used
    another thread is waiting on it too 
    */
    if((*s) < 0)
    {
        // give the blocked field the reason why the semaphore is blocked
        CurrentlyRunningThread -> blocked = s;
        EndCriticalSection(IBit_State);

        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
        return;
    }

    EndCriticalSection(IBit_State);
}

// G8RTOS_SignalSemaphore
// Signals that the semaphore has been released by incrementing the value by 1.
// This is a critical section!
// Goes through a linked list of all TCBs and wakes up the first one that has a blocked field equal to the semaphore
// Wakes it up by setting the blocked field to 0
void G8RTOS_SignalSemaphore(semaphore_t* s) {
    IBit_State = StartCriticalSection();
    (*s)++;
    if((*s) <= 0)
    {
        tcb_t *pt = CurrentlyRunningThread -> nextTCB;
        while(pt->blocked != s)
        {
            pt = pt -> nextTCB;
        }
        pt -> blocked = 0;
    }
    EndCriticalSection(IBit_State);
}
/********************************Public Functions***********************************/
