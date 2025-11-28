/*
 * multimod_BME280.c
 *
 *  Created on: Sep 11, 2024
 *      Author: hamid
 */


/************************************Includes***************************************/

#include <stdint.h>

#include "../multimod_i2c.h"
#include "../multimod_BME280.h"

/************************************Includes***************************************/


void BME280_Init() {}

void BME280_WriteRegister(uint8_t addr, uint8_t data) {}

uint32_t BME280_ReadOutput(uint8_t addr) {}

uint16_t BME280_ReadDig1(void) {}

uint16_t BME280_ReadDig2(void) {}

uint16_t BME280_ReadDig3(void) {}

uint32_t BME280_ReadTemp(void) {}

uint8_t BME280_ID(void) {}

int32_t BME280_Convert(uint32_t adc_T) {}

