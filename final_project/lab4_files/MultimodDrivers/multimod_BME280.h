/*
 * multimod_BME280.h
 *
 *  Created on: Sep 11, 2024
 *      Author: hamid
 */

#ifndef MULTIMODDRIVERS_MULTIMOD_BME280_H_
#define MULTIMODDRIVERS_MULTIMOD_BME280_H_

#include <stdint.h>

void BME280_Init();

void BME280_WriteRegister(uint8_t addr, uint8_t data);

uint32_t BME280_ReadOutput(uint8_t addr);

uint32_t BME280_ReadTemp(void);

uint8_t BME280_ID(void);

uint16_t BME280_ReadDig1(void);

uint16_t BME280_ReadDig2(void);

uint16_t BME280_ReadDig3(void);

int32_t BME280_Convert(uint32_t adc_T);




#endif /* MULTIMODDRIVERS_MULTIMOD_BME280_H_ */
