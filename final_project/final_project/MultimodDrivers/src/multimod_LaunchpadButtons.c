// multimod_OPT3001.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 functions

/************************************Includes***************************************/

#include "../multimod_LaunchpadButtons.h"

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>


/************************************Includes***************************************/

/********************************Public Functions***********************************/

// LaunchpadButtons_Init
// Initializes the GPIO port & pins necessary for the button switches on the
// launchpad.
// Return: void
void LaunchpadButtons_Init() {
     // Enable port F for switches
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // write unlock key for GPIO lock and commit changes
    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_0);

    // Use SW1 & SW2, configure as inputs.
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4));

    // Disable interrupts
    GPIOIntDisable(GPIO_PORTF_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_4));

    // Clear interrupts 
    GPIOIntClear(GPIO_PORTF_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_4));

    // Set interrupt type to falling edge
    GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4), GPIO_FALLING_EDGE);

    // Enable the GPIO Interrupt
    GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_4));

    // Enable global interrupts
    // IntEnable(INT_GPIOF);
}

// LaunchpadButtons_ReadSW1
// Checks to see if button 1 is pressed. If it is, return 1; else return 0.
// Return: uint8_t
uint8_t LaunchpadButtons_ReadSW1() {
   // GPIOPinRead() may be of use
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0)
    {
        // wait until pin in released
        while(!(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4));
        return 1;
    }
    else{return 0;}
}

// LaunchpadButtons_ReadSW2
// Checks to see if button 1 is pressed. If it is, return 1; else return 0.
// Return: uint8_t
uint8_t LaunchpadButtons_ReadSW2() {
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0)
    {
        while(!(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == GPIO_PIN_0));
        return 1;
    }
    else{return 0;}
}



/********************************Public Functions***********************************/
