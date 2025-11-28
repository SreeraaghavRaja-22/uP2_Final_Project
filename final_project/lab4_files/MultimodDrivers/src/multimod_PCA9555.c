// multimod_OPT3001.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 functions

/************************************Includes***************************************/

#include "../multimod_PCA9555.h"

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

uint16_t PCA9555_GetInput(uint32_t mod, uint8_t addr) {
    uint8_t data[2];
    // write to the 
    
    return;
}

void PCA9555_SetPinDir(uint32_t mod, uint8_t addr, uint16_t pins) {

    uint8_t data[2] = {(uint8_t)(pins>>8),(uint8_t)pins};

    I2C_WriteMultiple(mod, addr, data, 2);
}

void PCA9555_SetPinPol(uint32_t mod, uint8_t addr, uint16_t pins) {

    uint8_t data[2] = {(uint8_t)(pins>>8),(uint8_t)pins};

    // invert pins because active-low
    I2C_WriteMultiple(mod, addr, data, 2);
}
void PCA9555_SetOutput(uint32_t mod, uint8_t addr, uint16_t pins) {

    uint8_t data[2] = {(uint8_t)(pins>>8),(uint8_t)pins};
    
    // initialize the multimod buttons pins as outputs 
    I2C_WriteMultiple(mod, addr, data, 2);
}



uint16_t PCA9555_ReadReg(uint32_t mod, uint8_t gpio_module_addr, uint8_t addr) {
    // your code
}

/********************************Public Functions***********************************/
