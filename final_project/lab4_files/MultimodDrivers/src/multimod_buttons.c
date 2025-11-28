// multimod_buttons.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for button functions

/************************************Includes***************************************/

#include "../multimod_i2c.h"
#include "../multimod_buttons.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>
/************************************Includes***************************************/

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init() {

    I2C_Init(I2C_A_BASE);

    // enable the GPIO buttons 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){}

    // disable interrupts before configuring 
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);

    // set the interrupt Pin as an input from the perspective of the launchpad
    GPIOPinTypeGPIOInput(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);

    // set the type of interrupt to falling edge
    GPIOIntTypeSet(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE);

    // Clear interrupt pin 
    GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    
    // Enable the pin interrupts 
    GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t 
uint8_t MultimodButtons_Get() {
    // read GPIO Expander and address 0
    I2C_WriteSingle(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR, 0);
    return (I2C_ReadSingle(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR));
}

/********************************Public Functions***********************************/

