/**
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
#include <stdio.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "esp_mac.h"

#include "Globals.h"
#include "HardwareConfig.h"
#include "PSensorDrv.h"
#include "lps28dfw_reg.h"
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

#define I2C_SLAVE_ADDRESS                   0x5C
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
static INT8U buffer[1000];
static lps28dfw_data_t data;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/

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
        .master.clk_speed = 10000,
    };
    i2c_param_config(I2C_MASTER_PORT, &i2cconf);
    i2c_driver_install(I2C_MASTER_PORT, i2cconf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
********************************************************************************
* @brief    Sensor Write
* @param    reg = register address to read from
            data = data to write to
            len = number of bytes to write
* @return   none
* @remark   Used to write data to sensor
********************************************************************************
*/
void Sensor_Write(INT8U reg, INT8U *data, INT16U len) {
    INT8U writeBuf[len + 1];
    writeBuf[0] = reg;
    memcpy(&writeBuf[1], data, len);
    // Writing register address and data
    i2c_master_write_to_device(I2C_MASTER_PORT, I2C_SLAVE_ADDRESS,
        writeBuf, len + 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
********************************************************************************
* @brief    Sensor Read
* @param    reg = register address to read from
            buf = buffer to hold received data
            len = expected number of bytes received
* @return   none
* @remark   Used to read data from sensor
********************************************************************************
*/
void Sensor_Read(INT8U reg, INT8U *buf, INT16U len) {
    // Writing register address
    i2c_master_write_to_device(I2C_MASTER_PORT, I2C_SLAVE_ADDRESS,
        &reg, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    // Read data from register
    i2c_master_read_from_device(I2C_MASTER_PORT, I2C_SLAVE_ADDRESS,
        buf, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

