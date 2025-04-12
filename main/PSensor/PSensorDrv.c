/**
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

#include "Globals.h"
#include "HardwareConfig.h"
#include "PSensorDrv.h"
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#define I2C_MASTER_PORT                     I2C_NUM_0

#define I2C_DEVICE_ADDRESS                  0x5D
#define I2C_DEVICE_CTRLREG1                 0x10
#define I2C_DEVICE_HREG                     0x2A
#define I2C_DEVICE_LREG                     0x29
#define I2C_DEVICE_XLREG                    0x28

#define I2C_DEVICE_DATARATE                 0x18
/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
static int Sensor_readReg(INT8U reg);
static void Sensor_writeReg(INT8U data, INT8U reg);
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Sensor Init
* @param    none
* @return   none
* @remark   Used initialize settings for sensor
********************************************************************************
*/
void Sensor_Init(void) {
    Sensor_writeReg(I2C_DEVICE_DATARATE, I2C_DEVICE_CTRLREG1);
}

/**
********************************************************************************
* @brief    Sensor Read Pressure
* @param    none
* @return   pressure value in hPA
* @remark   Used to read pressure data from sensor
********************************************************************************
*/
INT16U Sensor_ReadPressure(void) {
    INT32U total = 0;
    total |= Sensor_readReg(I2C_DEVICE_HREG) << 16;
    total |= Sensor_readReg(I2C_DEVICE_LREG) << 8;
    total |= Sensor_readReg(I2C_DEVICE_XLREG);
    total /= 4096;
    return total;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

// reading from a specific register from the sensor
int Sensor_readReg(INT8U reg) {
    INT8U data = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    
    i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return data;
}

// writing to a specific register from the sensor
void Sensor_writeReg(INT8U data, INT8U reg) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    
    i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
}