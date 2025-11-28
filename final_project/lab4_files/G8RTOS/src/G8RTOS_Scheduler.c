// G8RTOS_Scheduler.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for scheduler functions

#include "../G8RTOS_Scheduler.h"
#include "../G8RTOS_Structures.h"

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include "../G8RTOS_CriticalSection.h"

#include <inc/hw_memmap.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/************************************Includes***************************************/

/********************************Private Variables**********************************/

// Thread Control Blocks - array to hold information for each thread
static tcb_t threadControlBlocks[MAX_THREADS];

// Thread threadStacks - array of arrays for individual stacks of each thread
static uint32_t threadStacks[MAX_THREADS][STACKSIZE];

// Periodic Event Threads - array to hold pertinent information for each thread
static ptcb_t ptcbs[MAX_PTHREADS];

// Current Number of Threads currently in the scheduler
static uint32_t NumberOfThreads;

// Current Number of Periodic Threads currently in the scheduler
static uint32_t NumberOfPThreads;

// Index of the first dead thread for relinking
static uint32_t deadThreadIndex;

static char* debugName;




/********************************Private Variables**********************************/

/*******************************Private Functions***********************************/

// Occurs every 1 ms.
static void InitSysTick(void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1000);
    SysTickIntRegister(SysTick_Handler);
    IntRegister(FAULT_PENDSV, PendSV_Handler);
    SysTickIntEnable();
    SysTickEnable();
}

/*******************************Private Functions***********************************/


/********************************Public Variables***********************************/

uint32_t SystemTime;

tcb_t* CurrentlyRunningThread;

volatile uint8_t aliveCount = 0;



/********************************Public Variables***********************************/



/********************************Public Functions***********************************/

// SysTick_Handler
// Increments system time, sets PendSV flag to start scheduler.
// Return: void
void SysTick_Handler() {
    SystemTime++;

    tcb_t* currThread = CurrentlyRunningThread;
    ptcb_t* currPThread = &ptcbs[0];

   
    // loop through all the periodic threads and execute them appropriately (if their time is now)
    do{
        
        if(SystemTime >= currPThread->executeTime){
            // call the handler (calls the function execution)
            currPThread->handler();
            // update current time 
            currPThread->currentTime = SystemTime;
            // update the execute time with the period (time between each thread) + current time (could handle the offset too)
            currPThread->executeTime = currPThread->currentTime + currPThread->period;
        }

        currPThread = currPThread->nextPTCB;
    }while(currPThread != &ptcbs[0]);
    

    //Loop through the background threads: check sleeping threads and wake them up appropriately if their time is now
    while(!currThread->isAlive)
    {
        currThread = currThread->nextTCB;
    }
    tcb_t* temp = currThread;

    do
    {
        if((SystemTime >= currThread -> sleepCount) && (currThread -> sleepCount != 0)) // != or >= works but it's uint32_t so != should be chill
        {
            currThread -> asleep = false;
            currThread -> sleepCount = 0; 
        }
        currThread = currThread -> nextTCB;
    } while (temp != currThread);
    

    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

// G8RTOS_Init
// Initializes the RTOS by initializing system time.
// Return: void
void G8RTOS_Init() {
    //uint32_t newVTORTable = 0x20000000;
    //uint32_t* newTable = (uint32_t*) newVTORTable;
    //uint32_t* oldTable = (uint32_t*) 0;
    //HWREG(NVIC_VTABLE) = newVTORTable;

    SystemTime = 0;
    NumberOfThreads = 0;
    NumberOfPThreads = 0;
    deadThreadIndex = 0;
    
}

// G8RTOS_Launch
// Launches the RTOS.
// Return: error codes, 0 if none
int32_t G8RTOS_Launch() {
    InitSysTick();
    CurrentlyRunningThread = &threadControlBlocks[0];
    IntPrioritySet(FAULT_SYSTICK, 0xE0);
    IntPrioritySet(FAULT_PENDSV, 0xE0);
    G8RTOS_Start(); // call the assembly function
    return 0;
}

// G8RTOS_Scheduler
// Chooses next thread in the TCB. This time uses priority scheduling.
// Return: void
void G8RTOS_Scheduler() {

    // define the max priority 
    uint8_t max = 255;
    
    // define two threads that are the thread and current thread that we're looking at
    tcb_t* pt; 
    tcb_t* bestPt;
    tcb_t* temp;

    // set the thread we're looking at to the currently running thread
    pt = CurrentlyRunningThread; 

    // check if the current thread is alive
    while(!pt->isAlive)
    {
        pt=pt->nextTCB;
    }

    temp = pt; 

    // have a do while loop so that we can at least go through the loop once 
    // this loops through all the threads and finds the next one highest priority one that is neither asleep or blocked
    do
    {
        pt = pt -> nextTCB; 

        // if it's strictly less than, then if the only available thread is the same priority level then this fails 
        if((pt->priority <= max) && ((pt->blocked) == 0) && (pt->sleepCount == 0) && (pt->isAlive))
        {
            max = pt->priority;
            bestPt = pt;
        }

    }while(temp != pt); 

    // update the value of the CRT to the bestPT
    CurrentlyRunningThread = bestPt;
}

// G8RTOS_AddThread
// Adds a thread. This is now in a critical section to support dynamic threads.
// It also now should initalize priority and account for live or dead threads.
// Param void* "threadToAdd": pointer to thread function address
// Param uint8_t "priority": priority from [0, 255].
// Param char* "name": character array containing the thread name.
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *name, threadID_t ID) {
    
    uint32_t currDeadIndex = MAX_THREADS-1;
    uint32_t nextAliveIndex = 0;
    uint32_t prevAliveIndex = MAX_THREADS-1;

    // find the index of the first available dead thread to overwrite
    for(int i = MAX_THREADS-1; i>=0; i--){     
        // find the index of the first dead thread   
        if(!(threadControlBlocks[i].isAlive) && i < currDeadIndex){
            currDeadIndex = nextAliveIndex = prevAliveIndex = i;
        }
    }

    // check for the next alive index
    for(int j = 1; j < MAX_THREADS; j++){
        // wrap around by incrementing and going back down to 0 after 
        uint32_t currInd = (currDeadIndex + j) % MAX_THREADS;
        if(threadControlBlocks[currInd].isAlive){
            // pick closes next alive thread and break
            nextAliveIndex = currInd;
            break;
        }
    }

    // check for the previously alive index
    for(int j = 1; j < MAX_THREADS; j++){
        // wrap around by subtracting the current index by the iterator and mod with number of threads
        uint32_t currInd = (currDeadIndex - j + MAX_THREADS) % MAX_THREADS;
        if(threadControlBlocks[currInd].isAlive){
            // pick closest / nearest previous alive thread and break
            prevAliveIndex = currInd;
            break;
        }
    }


    IBit_State = StartCriticalSection();
    
    // check that num_threads == max_threads {return error code }
    if(aliveCount == MAX_THREADS){
        
        // end critical section early and exist function
        EndCriticalSection(IBit_State);

        // return error code
        return THREAD_LIMIT_REACHED;
    } 

    // added always points to the first thread (next)
    threadControlBlocks[currDeadIndex].nextTCB = &threadControlBlocks[nextAliveIndex];
    // first always points to the added (prev)
    threadControlBlocks[nextAliveIndex].previousTCB = &threadControlBlocks[currDeadIndex];
    // case when you're adding threads
    // previously / intermediate thread points to most recently added thread
    threadControlBlocks[prevAliveIndex].nextTCB = &threadControlBlocks[currDeadIndex];        
        // most recently added thread poipnts to previously added thread 
    threadControlBlocks[currDeadIndex].previousTCB = &threadControlBlocks[prevAliveIndex];


    // initialize the stack of the current thread
    SetInitialStack(currDeadIndex);

    // set the program counter value to the value of the thread that we need
    threadStacks[currDeadIndex][STACKSIZE-2] = (int32_t)threadToAdd;
    
    threadControlBlocks[currDeadIndex].priority = priority;
    threadControlBlocks[currDeadIndex].blocked = 0; 
    threadControlBlocks[currDeadIndex].asleep = false; 
    threadControlBlocks[currDeadIndex].ThreadID = ID;


    threadControlBlocks[currDeadIndex].isAlive = true;

    // increment the aliveCount value -- useful for KillThread
    aliveCount++;

    threadControlBlocks[currDeadIndex].sleepCount = 0;

    // set the thread name 
    strcpy(threadControlBlocks[currDeadIndex].threadName, name);

    debugName = name; 

    // increment the number of threads at the end
    NumberOfThreads++; 
    
    EndCriticalSection(IBit_State);

    return NO_ERROR;
}

// G8RTOS_Add_APeriodicEvent
// Param void* "AthreadToAdd": pointer to thread function address
// Param uint8_t "priority": Priority of aperiodic event, [1..6]
// Param int32_t "IRQn": Interrupt request number that references the vector table. [0..155].
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_APeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, int32_t IRQn) {
    IBit_State = StartCriticalSection();
    // check for the appropriate interrupt levels and the specific priority of the thread
    
    if(IRQn > MAX_INTERRUPTS || IRQn < 1){
        EndCriticalSection(IBit_State);
        return IRQn_INVALID;
    }
    if(priority < 1 || priority > OSINT_PRIORITY){
        EndCriticalSection(IBit_State);
        return HWI_PRIORITY_INVALID;
    }
    
    // register the interrupt
    IntRegister(IRQn, AthreadToAdd);
    
    // set the priority 
    IntPrioritySet(IRQn, priority);

    // enable the interrupt 
    IntEnable(IRQn);

    EndCriticalSection(IBit_State);
    return NO_ERROR;
}

// G8RTOS_Add_PeriodicEvent
// Adds periodic threads to G8RTOS Scheduler
// Function will initialize a periodic event struct to represent event.
// The struct will be added to a linked list of periodic events
// Param void* "PThreadToAdd": void-void function for P thread handler
// Param uint32_t "period": period of P thread to add
// Param uint32_t "execution": When to execute the periodic thread
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_PeriodicEvent(void (*PThreadToAdd)(void), uint32_t period, uint32_t execution) {
    if(NumberOfPThreads >= MAX_PTHREADS){
        return THREAD_LIMIT_REACHED;
    }

    // setting the doubly linked linked list for Periodic Threads
    ptcbs[NumberOfPThreads].nextPTCB = &ptcbs[0];
    ptcbs[0].previousPTCB = &ptcbs[NumberOfPThreads];

    if(NumberOfPThreads != 0){
        ptcbs[NumberOfPThreads-1].nextPTCB = &ptcbs[NumberOfPThreads];
        ptcbs[NumberOfPThreads].previousPTCB = &ptcbs[NumberOfPThreads-1];
    }

    ptcbs[NumberOfPThreads].handler = PThreadToAdd;
    ptcbs[NumberOfPThreads].period = period; 
    ptcbs[NumberOfPThreads].executeTime = execution;

    NumberOfPThreads++;

    return NO_ERROR;
}

// G8RTOS_KillThread
// Param uint32_t "threadID": ID of thread to kill
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_KillThread(threadID_t threadID) {
    IBit_State = StartCriticalSection();
    // Loop through tcb. If not found, return thread does not exist error. If there is only one thread running, don't kill it.
    tcb_t* pt = CurrentlyRunningThread; 
   
    do{
        pt = pt->nextTCB;
        if(pt->ThreadID == threadID){
            if(aliveCount > 1){

                // kill the currently running thread
                pt->isAlive = false; 

                // decrement the count for the number of alive Threads 
                aliveCount--;

                // fix the linked list logic here 
                tcb_t* prev = pt->previousTCB; 
                tcb_t* next = pt->nextTCB; 
                prev->nextTCB = next; 
                next->previousTCB = prev; 
                
                // end critical section
                EndCriticalSection(IBit_State);
                return NO_ERROR; 
            } 
            else if(aliveCount == 1){
                EndCriticalSection(IBit_State);
                return CANNOT_KILL_LAST_THREAD;
            }
        }
    }while(pt != CurrentlyRunningThread);

    // thread not found if the loop finishes without exiting the function in some way
   	EndCriticalSection(IBit_State);
   	return THREAD_DOES_NOT_EXIST;
}

// G8RTOS_KillSelf
// Kills currently running thread.
// Return: sched_ErrCode_t
// Get this to work, the issue is that changing the CRT's values will prevent it from updating in the sche
sched_ErrCode_t G8RTOS_KillSelf() {
    G8RTOS_KillThread(CurrentlyRunningThread->ThreadID);
    HWREG(NVIC_INT_CTRL) |= (NVIC_INT_CTRL_PEND_SV);
    return NO_ERROR;
}

// sleep
// Puts current thread to sleep
// Param uint32_t "durationMS": how many systicks to sleep for
void sleep(uint32_t durationMS) {
    CurrentlyRunningThread -> sleepCount = durationMS + SystemTime; 
    CurrentlyRunningThread -> asleep = true;
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

// G8RTOS_GetThreadID
// Gets current thread ID.
// Return: threadID_t
threadID_t G8RTOS_GetThreadID(void) {
    // Returns the thread ID
    return CurrentlyRunningThread->ThreadID;        
}

// G8RTOS_GetNumberOfThreads
// Gets number of threads.
// Return: uint32_t
uint32_t G8RTOS_GetNumberOfThreads(void) {
    // Returns the number of threads
    return NumberOfThreads;         
}


void SetInitialStack(int i){
    // stack builds down, so stack pointer initialized to "top of stack" which is at the bottom since the stack builds down 
    // just sets the stack for each thread with fake news
    threadControlBlocks[i].stackPointer = &threadStacks[i][STACKSIZE-16]; // thread stack pointer
    threadStacks[i][STACKSIZE-1] = 0x01000000; // Thumb bit -- could also use the Group Configuration PSR

    // LR = R14 and R12
    threadStacks[i][STACKSIZE-3] = 0x14141414; // R14
    threadStacks[i][STACKSIZE-4] = 0x12121212; // R12

    // Hardware Stack Registers
    threadStacks[i][STACKSIZE-5] = 0x03030303; // R3
    threadStacks[i][STACKSIZE-6] = 0x02020202; // R2
    threadStacks[i][STACKSIZE-7] = 0x01010101; // R1
    threadStacks[i][STACKSIZE-8] = 0x00000000; // R0

    // Scratch registers???
    threadStacks[i][STACKSIZE-9] = 0x11111111; // R11
    threadStacks[i][STACKSIZE-10] = 0x10101010; // R10
    threadStacks[i][STACKSIZE-11] = 0x09090909; // R9
    threadStacks[i][STACKSIZE-12] = 0x08080808; // R8
    threadStacks[i][STACKSIZE-13] = 0x07070707; // R7
    threadStacks[i][STACKSIZE-14] = 0x06060606; // R6
    threadStacks[i][STACKSIZE-15] = 0x05050505; // R5
    threadStacks[i][STACKSIZE-16] = 0x04040404; // R4
}
/********************************Public Functions***********************************/
