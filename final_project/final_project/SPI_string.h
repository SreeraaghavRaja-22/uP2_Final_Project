/*
 * SPI_string.h
 *
 *  Created on: Sep 25, 2025
 *      Author: sreer
 */

#ifndef SPI_STRING_H_
#define SPI_STRING_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Draw a null-terminated string starting at (x, y) with the given text color.
void ST7789_DrawStringStatic(const char* string, const uint16_t text_color, const uint8_t x, const uint8_t y);

// Draw a single character at (x, y) with the given text color.
void ST7789_DrawCharStatic(const char c, const uint16_t text_color, const uint8_t x, const uint8_t y);

void reverse(char str[], int length);

// Implementation of citoa()
char* citoa(int num, char* str, int base);


#endif /* SPI_STRING_H_ */
