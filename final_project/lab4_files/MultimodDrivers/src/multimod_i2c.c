// multimod_i2c.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for I2C functions

/************************************Includes***************************************/
#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>
/************************************Includes***************************************/

/********************************Public Functions***********************************/

// I2C_Init
// Initializes specified I2C module
// Param uint32_t "mod": base address of module
// Return: void
void I2C_Init(uint32_t mod) {
    // your code here
    if(mod == I2C_A_BASE)
    {  
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

        // Configure the pins as SCL and SDA
        GPIOPinConfigure(GPIO_PA6_I2C1SCL);
        GPIOPinConfigure(GPIO_PA7_I2C1SDA);

        GPIOPinTypeI2CSCL(I2C_A_GPIO_BASE, I2C_A_PIN_SCL);

        GPIOPinTypeI2C(I2C_A_GPIO_BASE, I2C_A_PIN_SDA);
    }

    if(mod == I2C_B_BASE)
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        // Configure the pins as SCL and SDA
        GPIOPinConfigure(GPIO_PB2_I2C0SCL);
        GPIOPinConfigure(GPIO_PB3_I2C0SDA);

        GPIOPinTypeI2CSCL(I2C_B_GPIO_BASE, I2C_B_PIN_SCL); 

        GPIOPinTypeI2C(I2C_B_GPIO_BASE, I2C_B_PIN_SDA);

        
    }
   
    // you get this for free -- this does what the RCGCI2C register also does
    I2CMasterInitExpClk(mod, SysCtlClockGet(), false);   
    
    return;
}

// I2C_WriteSingle
// Writes a single byte to an address.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t "byte": byte to send
// Return: void
void I2C_WriteSingle(uint32_t mod, uint8_t addr, uint8_t byte) {
    // which module and where to?
    // should be module 1 and to the address first in MSB first order
    // initialize the slave, false indicates a write to the slave
    I2CMasterSlaveAddrSet(mod, addr, false);

    // what data? -- the byte 
    I2CMasterDataPut(mod, byte);   

    // what mode?
    // how do I generate the start condition here
    I2CMasterControl(mod, I2C_MASTER_CMD_SINGLE_SEND);
    
    // should we wait till finished?
    // delay while master is busy 
    while(I2CMasterBusy(mod));

    return;
}

// I2C_ReadSingle
// Reads a single byte from address.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Return: uint8_t
uint8_t I2C_ReadSingle(uint32_t mod, uint8_t addr) {
    // your code here
     // which module and where to? 
    // the true indicates that the master is initiating a read from slave
    I2CMasterSlaveAddrSet(mod, addr, true);

    // what data? (not sending any data from master)
    // what mode? -- Single Receive
    I2CMasterControl(mod, I2C_MASTER_CMD_SINGLE_RECEIVE);

    // should be wait till finished?
    // delay while master is busy
    while(I2CMasterBusy(mod));

    // return data from module
    return ((uint8_t)I2CMasterDataGet(mod));
}

// I2C_WriteMultiple
// Write multiple bytes to a device.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to transmit
// Return: void
void I2C_WriteMultiple(uint32_t mod, uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // which module and where to? 
    // same device and address as the single write
    I2CMasterSlaveAddrSet(mod, addr, false);

    // what data?
    I2CMasterDataPut(mod, *data);

    // what mode?
    // initialize the start of the burst send 
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_START);

    // should be wait till finished?
    while(I2CMasterBusy(mod));
    

    int i = 0;
    // hint: how many times do we need to write? 
    // start at 1 because we already wrote once (lowkey kinda vital)
    for(i = 1; i < num_bytes; i++){
        if(i == (num_bytes - 1))
        {

            // hint 2: how do we tell the module we're done sending data? 
            I2CMasterDataPut(mod, *(data + (num_bytes - 1)));
            I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_FINISH);
            while(I2CMasterBusy(mod));
        }

        else{
            I2CMasterDataPut(mod, *(data + i));
            I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_CONT);
            while(I2CMasterBusy(mod));
        } 
    }
    return;
}

// I2C_ReadMultiple
// Read multiple bytes from a device.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void I2C_ReadMultiple(uint32_t mod, uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // your code here
    // which module and where to? 
    // same device and address as the single read
    I2CMasterSlaveAddrSet(mod, addr, true);

    // what data? -- None 
    // what mode?
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_START);

    // should be wait till finished?
    while(I2CMasterBusy(mod));

    // put received data into a temp register
     *(data) = I2CMasterDataGet(mod) & 0xFF;
    // hint: how many times do we need to read? 
    int i = 0;
    for(i = 1; i < num_bytes; i++)
    {
        if(i == (num_bytes-1))
        {
             // hint 2: how do we tell the module we're done receiving data?
            I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
            while(I2CMasterBusy(mod));
            // anding with 0xFF truncates value from uint32 to uint8 
            // having (num_bytes - 1) makes code more readable for me
            *(data + (num_bytes - 1)) = I2CMasterDataGet(mod) & 0xFF; 

        }
        else{
            I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            while(I2CMasterBusy(mod));
            *(data + i) = I2CMasterDataGet(mod) & 0xFF; 
        }
        
    }

    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    return;
}

/********************************Public Functions***********************************/

