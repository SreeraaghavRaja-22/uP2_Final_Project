// multimod_joystick.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for joystick functions

/************************************Includes***************************************/


#include "../multimod_joystick.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/adc.h>
#include <driverlib/interrupt.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <inc/hw_gpio.h>

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// JOYSTICK_Init
// Initializes ports & adc module for joystick
// Return: void
void JOYSTICK_Init(void) {
    // multimod buttons use the same port as joystick x, y, so put multimod buttons init before joystick init

    // enable ADC module (Idk if it's 0 or 1) -- shouldn't matter (might have to use both)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}

    // configure pins as ADC pins
    GPIOPinTypeADC(GPIO_PORTE_BASE, (GPIO_PIN_2 | GPIO_PIN_3));

    // Configure the clock for the ADC (might not be necessary)

    // set the reference for the ADC module (probably not necessary)
    ADCSequenceDisable(JOYSTICK_ADC_BASE, 0);
    ADCIntDisable(JOYSTICK_ADC_BASE, 0);
    ADCSequenceConfigure(JOYSTICK_ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    // read the x and y values by making them successive steps in the ADC sequence sampler (need a manual trigger)
    ADCSequenceStepConfigure(JOYSTICK_ADC_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE);
    ADCSequenceStepConfigure(JOYSTICK_ADC_BASE, 0, 1, ADC_CTL_CH1 | ADC_CTL_END | ADC_CTL_IE);

    ADCSequenceEnable(JOYSTICK_ADC_BASE, 0);

    // enable the joystick interrupt
    JOYSTICK_IntEnable();
}



// JOYSTICK_IntEnable
// Enables interrupts
// Return: void
void JOYSTICK_IntEnable() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){}

    // configure the interrupt pin for the switch on joystick 
    GPIOPinTypeGPIOInput(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);

    GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    GPIOIntTypeSet(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
}

// JOYSTICK_GetPress
// Gets button reading
// Return: bool
uint8_t JOYSTICK_GetPress() {
    return ((uint8_t)GPIOPinRead(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN));
}

// JOYSTICK_GetX
// Gets X adc reading from joystick
// Return: uint16_t
uint16_t JOYSTICK_GetX() {
    
    uint32_t data = JOYSTICK_GetXY();

    return ((uint16_t)(data >> 16));
}

// JOYSTICK_GetY
// Gets Y adc reading from joystick
// Return: uint16_t
uint16_t JOYSTICK_GetY() {
    // do some ADC stuff
    uint32_t data = JOYSTICK_GetXY();

    return ((uint16_t)(data));
}


// JOYSTICK_GetXY
// Gets X and Y adc readings
// Return: uint32_t, 16-bit packed, upper 16-bit is X and lower 16-bit is Y.
uint32_t JOYSTICK_GetXY() {
    uint32_t data[2];
    // trigger ADC interrupt manually 
    ADCProcessorTrigger(JOYSTICK_ADC_BASE, 0);
    // wait for sample sequence to complete 
    while(!(ADCIntStatus(JOYSTICK_ADC_BASE, 0, false)));

    // Clear interrupt flag 
    ADCIntClear(JOYSTICK_ADC_BASE, 0);

    // get the data (FIFO[0] has X data and FIFO[1] has Y data )
    ADCSequenceDataGet(JOYSTICK_ADC_BASE, 0, data);

    uint32_t XYData = (((uint32_t)(data[0]) << 16) | (uint32_t)(data[1]));

    return XYData;
}

/********************************Public Functions***********************************/

