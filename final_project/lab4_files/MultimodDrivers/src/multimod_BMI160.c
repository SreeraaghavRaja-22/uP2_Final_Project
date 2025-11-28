// multimod_BMI160.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for BMI160 functions

/************************************Includes***************************************/



#include <stdint.h>
#include "../multimod_i2c.h"
#include "../multimod_BMI160.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// BMI160_Init
// Initializes the BMI160. Currently enables the accelerometer
// in full-power mode.
//0x69
// Return: void
void BMI160_Init() {
    I2C_Init(I2C_A_BASE);

    // Power on accelerometer
    // your code here
    BMI160_WriteRegister(BMI160_CMD_ADDR, BMI160_ACCEL_NORM_PWR);

    // Power on gyroscope 
    BMI160_WriteRegister(BMI160_CMD_ADDR, BMI160_GYRO_NORM_PWR);

    return;
}

// BMI160_WriteRegister
// Writes to a register address in the BMI160.
// Param uint8_t "addr": Register address
// Param uint8_t "data": data to write
// Return: void
void BMI160_WriteRegister(uint8_t addr, uint8_t data) {
    // with what? (hint youre writing only 1 data item, but you still need to specify where)
    // writing to device address which is 0x69 
    uint8_t temp[2] = {addr, data};
    uint8_t num_bytes = sizeof(temp);

    // write I2C device
    I2C_WriteMultiple(I2C_A_BASE, BMI160_ADDR, temp, num_bytes);
}

// BMI160_ReadRegister
// Reads from a register address in the BMI160.
// Param uint8_t "addr": Register address
// Return: void
uint8_t BMI160_ReadRegister(uint8_t addr) {
    // read from which addr?
    // read from the addr of the register we want to access of the BMI160
    // your code here
    // write the address of the register we want to read from 
    I2C_WriteSingle(I2C_A_BASE, BMI160_ADDR, addr);

    // was uint8_t data
    // should I cast this to a pointer type 
    uint8_t data = I2C_ReadSingle(I2C_A_BASE, BMI160_ADDR);

    // return the data
    return data; // your code here
}

// BMI160_MultiReadRegister
// Uses the BMI160 auto-increment function to read from multiple registers.
// Param uint8_t "addr": beginning register address
// Param uint8_t* "data": pointer to an array to store data in
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void BMI160_MultiReadRegister(uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // write to which addr?
    // write the address of the register we want to read from 
    I2C_WriteSingle(I2C_A_BASE, BMI160_ADDR, addr);

    // read the data from that addr
    // your code here
    I2C_ReadMultiple(I2C_A_BASE, BMI160_ADDR, data, num_bytes);
    return;
}

// BMI160_AccelXGetResult
// Gets the 16-bit x-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelXGetResult() {
    // your code here
    // if not read, wait till read

        // how do I wait until read -- poll for the status flag 
        // wait until the data ready for the accel flag is ready 
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));

    // read data
        uint8_t raw_accel_data[2];
        BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, raw_accel_data, 2);

    // write data -- probably not necesssary at first

    return ((int16_t)((raw_accel_data[1] << 8) | raw_accel_data[0]));
}

// BMI160_AccelYGetResult
// Gets the 16-bit y-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelYGetResult() {
    // your code here
    // poll DRDY ACC Flag
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));

    // read data
        uint8_t raw_accel_data[2];
        BMI160_MultiReadRegister(BMI160_DATA_O + ACCELY_O, raw_accel_data, 2);

    return ((int16_t)((raw_accel_data[1] << 8) | raw_accel_data[0]));
}

// BMI160_AccelZGetResult
// Gets the 16-bit z-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelZGetResult() {
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));

    // read data
        uint8_t raw_accel_data[2];
        BMI160_MultiReadRegister(BMI160_DATA_O + ACCELZ_O, raw_accel_data, 2);

    return ((int16_t)((raw_accel_data[1] << 8) | raw_accel_data[0]));
}

// BMI160_GyroXGetResult
// Gets the 16-bit x-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroXGetResult() {
    // your code here
    // if not read, wait till read
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

// read data
    uint8_t raw_gyr[2]; 
    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, raw_gyr, 2);

// write data 
    // your code here

   return ((int16_t)((raw_gyr[1] << 8) | raw_gyr[0]));
}

// BMI160_GyroYGetResult
// Gets the 16-bit y-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroYGetResult() {
    // your code here
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    // read data
    uint8_t raw_gyr[2]; 
    BMI160_MultiReadRegister(BMI160_DATA_O + GYROY_O, raw_gyr, 2);

    // write data 
        // your code here

   return ((uint16_t)((raw_gyr[1] << 8) | raw_gyr[0]));
}

// BMI160_GyroZGetResult
// Gets the 16-bit z-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroZGetResult() {
    // your code here
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    // read data
    uint8_t raw_gyr[2]; 
    BMI160_MultiReadRegister(BMI160_DATA_O + GYROZ_O, raw_gyr, 2);

    // write data 
        // your code here

   return ((uint16_t)((raw_gyr[1] << 8) | raw_gyr[0]));
}

// BMI160_MagXGetResult
// Gets the 16-bit x-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagXGetResult() {
    while(!BMI160_ReadRegister((BMI160_STATUS_ADDR) & BMI160_STATUS_DRDY_MAG));

    uint8_t raw_mag[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, raw_mag, 2);

    return((uint16_t)((raw_mag[1] << 8) | raw_mag[0]));
}

// BMI160_MagYGetResult
// Gets the 16-bit y-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagYGetResult() {
    while(!BMI160_ReadRegister((BMI160_STATUS_ADDR) & BMI160_STATUS_DRDY_MAG));

    uint8_t raw_mag[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGY_O, raw_mag, 2);

    return((uint16_t)((raw_mag[1] << 8) | raw_mag[0]));
}

// BMI160_MagZGetResult
// Gets the 16-bit z-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagZGetResult() {
    while(!BMI160_ReadRegister((BMI160_STATUS_ADDR) & BMI160_STATUS_DRDY_MAG));

    uint8_t raw_mag[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGZ_O, raw_mag, 2);

    return((uint16_t)((raw_mag[1] << 8) | raw_mag[0]));
}

// BMI160_AccelXYZGetResult
// Stores the 16-bit XYZ accelerometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_AccelXYZGetResult(uint16_t* data) {
    // if not read, wait till read
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));

    uint8_t raw[6]; 

    // read data
    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, raw, 6);

    // write data 
    // x data
    *(data) = (uint16_t)((raw[1] << 8) | raw[0]);
    *(data + 1) = (uint16_t)((raw[3] << 8) | raw[2]);
    *(data + 2) = (uint16_t)((raw[5] << 8) | raw[4]);

}

// BMI160_GyroXYZGetResult
// Stores the 16-bit XYZ gyroscope results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_GyroXYZGetResult(uint16_t* data) {
    // if not read, wait till read
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    uint8_t raw[6]; 

    // read data
    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, raw, 6);

    // write data 
    // x data
    *(data) = (uint16_t)((raw[1] << 8) | raw[0]);
    *(data + 1) = (uint16_t)((raw[3] << 8) | raw[2]);
    *(data + 2) = (uint16_t)((raw[5] << 8) | raw[4]);

    return;
}

// BMI160_MagXYZGetResult
// Stores the 16-bit XYZ magnetometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_MagXYZGetResult(uint16_t* data) {
    // if not read, wait till read
    while(!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG));

    uint8_t raw[6]; 

    // read data
    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, raw, 6);

    // write data 
    // x data
    *(data) = (uint16_t)((raw[1] << 8) | raw[0]);
    *(data + 1) = (uint16_t)((raw[3] << 8) | raw[2]);
    *(data + 2) = (uint16_t)((raw[5] << 8) | raw[4]);

    return;
}

// BMI160_GetDataStatus
// Gets the status register to determine if data is ready to read.
// Return: uint8_t
uint8_t BMI160_GetDataStatus(){
    return BMI160_ReadRegister(BMI160_STATUS_ADDR);
}

/********************************Public Functions***********************************/
