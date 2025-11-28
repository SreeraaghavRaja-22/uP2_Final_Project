#ifndef MULTIMOD_H_
#define MULTIMOD_H_

#include "multimod_uart.h"
#include "multimod_i2c.h"
#include "multimod_BMI160.h"
#include "multimod_OPT3001.h"
#include "multimod_LaunchpadButtons.h"
#include "multimod_LaunchpadLED.h"
#include "multimod_joystick.h"
#include "multimod_PCA9555.h"
#include "multimod_PCA9956b.h"
#include "multimod_ST7789.h"
#include "multimod_buttons.h"

static void Multimod_Init() {
    UART_Init();
    //BMI160_Init();
    //OPT3001_Init();
    //LaunchpadButtons_Init();
    //LaunchpadLED_Init();
    ST7789_Init();
    MultimodButtons_Init();
    JOYSTICK_Init();
}

#endif /* MULTIMOD_H_ */
