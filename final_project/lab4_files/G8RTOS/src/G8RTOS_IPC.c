// G8RTOS_IPC.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for FIFO functions for interprocess communication

#include "../G8RTOS_IPC.h"

/************************************Includes***************************************/

#include "../G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/******************************Data Type Definitions********************************/

#define FIFO_SIZE 16
#define MAX_NUMBER_OF_FIFOS 4

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

typedef struct G8RTOS_FIFO_t {
    int32_t buffer[FIFO_SIZE];
    int32_t* head;
    int32_t* tail; 
    uint32_t lostData; // used for debugging
    semaphore_t currentSize;
    semaphore_t mutex;

} G8RTOS_FIFO_t;

/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/

static G8RTOS_FIFO_t FIFOs[MAX_NUMBER_OF_FIFOS];

uint32_t volatile *putPt;
uint32_t volatile *getPt;
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_InitFIFO
// Initializes FIFO, points head & tail to relevant buffer
// memory addresses.
// Param "FIFO_index": Index of FIFO block
// Return: int32_t, -1 if error (i.e. FIFO full), 0 if okay
int32_t G8RTOS_InitFIFO(uint32_t FIFO_index) {

    if(FIFOs[FIFO_index].currentSize.count == FIFO_SIZE)
    {
        return -1;
    }
    // initialize head and tail addresses
    FIFOs[FIFO_index].head = FIFOs[FIFO_index].tail = &FIFOs[FIFO_index].buffer[0];
    // initialize lost data to 0
    FIFOs[FIFO_index].lostData = 0;
    // need to update the size of this semaphore
    G8RTOS_InitSemaphore(&FIFOs[FIFO_index].currentSize, 0);
    // in case multiple threads read the same FIFO
    G8RTOS_InitSemaphore(&FIFOs[FIFO_index].mutex, 1);

    // return 0 for successful initialization
    return 0;
}

// G8RTOS_ReadFIFO
// Reads data from head pointer of FIFO.
// Param "FIFO_index": Index of FIFO block
// Return: int32_t, data at head pointer
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index) {

    // wait for an item to be available (block if empty)
    G8RTOS_WaitSemaphore(&FIFOs[FIFO_index].currentSize);
    
    // wait for exclusive access
    G8RTOS_WaitSemaphore(&FIFOs[FIFO_index].mutex);
    
    // get the data (dereference head)
    int32_t data = *FIFOs[FIFO_index].head;

    // increment the head
    FIFOs[FIFO_index].head++;

    // if the value of head (stores and address) = address of final element in buffer array
    if(FIFOs[FIFO_index].head == &FIFOs[FIFO_index].buffer[FIFO_SIZE])
    {
        // set the value of head back to the start (wrap-around)
        FIFOs[FIFO_index].head = &FIFOs[FIFO_index].buffer[0];
    }
    

    // signal that the FIFO has been used to read
    G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].mutex);

    // signal that the data has been sent
    G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].currentSize);

    return data;
}

// G8RTOS_WriteFIFO
// Writes data to tail of buffer.
// Param "FIFO_index": Index of FIFO block
// Return: int32_t, data at head pointer
int32_t G8RTOS_WriteFIFO(uint32_t FIFO_index, uint32_t data) {
    // wait for exclusive access
    G8RTOS_WaitSemaphore(&FIFOs[FIFO_index].mutex);
    
    // wait for an item to be available 
    //G8RTOS_WaitSemaphore(&FIFOs[FIFO_index].currentSize);

    if(FIFOs[FIFO_index].currentSize.count > FIFO_SIZE - 1)
    {
        // increment lost data
        (FIFOs[FIFO_index].lostData)++;

        // overwrite old data 
        *(FIFOs[FIFO_index].tail) = data;
        *(FIFOs[FIFO_index].tail)++;

        if(FIFOs[FIFO_index].tail == &FIFOs[FIFO_index].buffer[FIFO_SIZE])
        {
            FIFOs[FIFO_index].tail = &FIFOs[FIFO_index].buffer[0];
        }

        G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].currentSize);
        G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].mutex);
        return -2;
    }
    else if(FIFO_index > MAX_NUMBER_OF_FIFOS - 1)
    {
        // out of bounds error
        G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].currentSize);
        G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].mutex);
        return -1;
    }

    // return FIFO Size
    *(FIFOs[FIFO_index].tail) = data; 
    *(FIFOs[FIFO_index].tail)++;

    if(FIFOs[FIFO_index].tail == &FIFOs[FIFO_index].buffer[FIFO_SIZE])
    {
        FIFOs[FIFO_index].tail = &FIFOs[FIFO_index].buffer[0];
    }

    
    G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].currentSize);
    G8RTOS_SignalSemaphore(&FIFOs[FIFO_index].mutex);

    return 0;
}

/********************************Public Functions***********************************/
