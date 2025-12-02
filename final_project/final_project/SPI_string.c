// Date Created: 2025-09-17
// Date Updated: 2025-09-18
// Spare String Functions (for SPI printing)

#include <stdint.h>
#include "font.h"
#include "SPI_string.h"
// i trust that you guys can figure out how to put the font in it's own header file (from the GFX library)
//  and that you can resolve other header dependencies by now on your own
// and that you can put these (the draw char and draw string) functions in a header file to expose to your toplevel

static uint8_t textsize = 2;
#define SEQUENTIAL_CHAR_SPACE 5

// écrire un string à un endroit précise
void ST7789_DrawStringStatic(const char* string, const uint16_t text_color, const uint8_t x, const uint8_t y) {
  // temp
  uint8_t temp_cursor_x = x;
  uint8_t temp_cursor_y = y;

  while(*string) {
    uint8_t c = (uint8_t)*string++;

    // ignorer lignes nouveaux
    if (c == '\n' || c == '\r') {
      return;
    }

    ST7789_DrawCharStatic(c, text_color, temp_cursor_x, temp_cursor_y);
    // temp_cursor_x += (1 - text_size) * SEQUENTIAL_CHAR_SPACE + SEQUENTIAL_CHAR_SPACE;
    temp_cursor_x += (SEQUENTIAL_CHAR_SPACE + textsize * 2.5 + (0.5 * textsize));
    temp_cursor_y += 0;
  }
}

void ST7789_DrawCharStatic(const char c, const uint16_t text_color, const uint8_t x, const uint8_t y) {
  uint8_t line;
  for (uint8_t i = 0; i < 5; i++) {
    line = font[c][i];

    for (uint8_t j = 0; j < 8; j++, line >>= 1) {
      if (line & 1) {
        if (textsize == 1) {
          ST7789_DrawPixel(x + i, y - j, text_color);
        } else {
          ST7789_DrawRectangle(x + i*textsize, y - j*textsize, textsize, textsize, text_color);
        }
      }
    }

  }
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}

// Implementation of citoa()
char* citoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}