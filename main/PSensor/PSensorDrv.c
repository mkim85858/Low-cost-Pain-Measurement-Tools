/**
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

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
#define I2C_MASTER_TX_BUF_DISABLE           0
#define I2C_MASTER_RX_BUF_DISABLE           0
#define I2C_MASTER_TIMEOUT_MS               1000

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
* @brief    I2C Init
* @param    none
* @return   none
* @remark   Used for initializing I2C at startup
********************************************************************************
*/
void I2C_Init(void) {
    i2c_config_t i2cconf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = LPS28_I2C_SDA,
        .scl_io_num = LPS28_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_MASTER_PORT, &i2cconf);
    i2c_driver_install(I2C_MASTER_PORT, i2cconf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

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
* @param    buf = buffer to hold received data
* @return   none
* @remark   Used to read pressure data from sensor
********************************************************************************
*/
void Sensor_ReadPressure(INT8U *buf) {
    buf[0] = Sensor_readReg(I2C_DEVICE_HREG);
    buf[1] = Sensor_readReg(I2C_DEVICE_LREG);
    buf[2] = Sensor_readReg(I2C_DEVICE_XLREG);
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