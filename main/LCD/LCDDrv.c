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
#include "LCDDrv.h"
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

#define I2C_DEVICE_ADDRESS                  0x3C
#define I2C_DEVICE_CTRL_COMMAND             0x00
#define I2C_DEVICE_CTRL_DATA                0x40

#define I2C_DEVICE_MULTIPLEX_RATIO          0xA8
#define I2C_DEVICE_DISPLAY_OFFSET           0xD3
#define I2C_DEVICE_DISPLAY_START_LINE       0x40
#define I2C_DEVICE_SET_SEGMENT_REMAP        0xA1
#define I2C_DEVICE_COM_DIRECTION            0xC8
#define I2C_DEVICE_COM_PINS                 0xDA
#define I2C_DEVICE_SET_MEMORY               0x20
#define I2C_DEVICE_SET_CONTRAST             0x81
#define I2C_DEVICE_RAM_CONTENT_DISPLAY      0xA4
#define I2C_DEVICE_NORMAL_DISPLAY           0xA6
#define I2C_DEVICE_SET_DISPLAY_CLOCK        0xD5
#define I2C_DEVICE_CHARGE_BUMP              0x8D
#define I2C_DEVICE_DISPLAY_ON               0xAF
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
static const INT8U progressBar[] = {
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
};

static const INT8U blankBar[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
static void LCD_writeCommand(INT8U* commands, INT8U length);
static void LCD_writeData(INT8U* data, INT8U length);
static void LCD_setCoord(INT8U page, INT8U col);
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    LCD Init
* @param    none
* @return   none
* @remark   Used to send initial commands to LCD
********************************************************************************
*/
void LCD_Init(void) {
    INT8U cmd[20] = {I2C_DEVICE_MULTIPLEX_RATIO, 0x3F, 
                     I2C_DEVICE_DISPLAY_OFFSET, 0x00, 
                     I2C_DEVICE_DISPLAY_START_LINE, 
                     I2C_DEVICE_SET_SEGMENT_REMAP, 
                     I2C_DEVICE_COM_DIRECTION,
                     I2C_DEVICE_COM_PINS, 0x12,
                     I2C_DEVICE_SET_MEMORY, 0x02,
                     I2C_DEVICE_SET_CONTRAST, 0x7F,
                     I2C_DEVICE_RAM_CONTENT_DISPLAY,
                     I2C_DEVICE_NORMAL_DISPLAY,
                     I2C_DEVICE_SET_DISPLAY_CLOCK, 0x80,
                     I2C_DEVICE_CHARGE_BUMP, 0x14,
                     I2C_DEVICE_DISPLAY_ON};
    LCD_writeCommand(cmd, 20);
    LCD_ClearScreen();
}
/**
********************************************************************************
* @brief    LCD Write Progress Bar
* @param    page : page to write progress bar (row)
            column : column to write progress bar
            type : if 1, write full bar, if 0, write blank bar
* @return   none
* @remark   Used to write a full or blank progress bar
********************************************************************************
*/
void LCD_WriteProgressBar(INT8U page, INT16U column, BOOLEAN type) {
    INT8U imgBuffer[24] = {0};
    for (INT16U i = 0; i < 24; i++) {
        if (type) {
            imgBuffer[i] = progressBar[i];
        }
        else {
            imgBuffer[i] = blankBar[i];
        }
    }
    for (INT8U i = 0; i < 2; i++) {
        LCD_setCoord(page + i, column);
        LCD_writeData(&imgBuffer[12 * i], 12);
    }
}
/**
********************************************************************************
* @brief    LCD Clear Page
* @param    page : page (row) to clear
* @return   none
* @remark   Used to clear a page on the LCD
********************************************************************************
*/
void LCD_ClearPage(INT8U page) {
    INT8U buffer[128] = {0};
    INT8U cmd[] = {0x00, 0x10, 0xB0 + page};
    LCD_writeCommand(cmd, 3);
    LCD_writeData(buffer, 128);
}
/**
********************************************************************************
* @brief    LCD Clear Screen
* @param    none
* @return   none
* @remark   Used to clear the entire screen on the LCD
********************************************************************************
*/
void LCD_ClearScreen(void) {
    INT8U buffer[128] = {0};
    for (INT8U i = 0; i < 8; i++) {
        INT8U cmd[] = {0x00, 0x10, 0xB0 + i};
        LCD_writeCommand(cmd, 3);
        LCD_writeData(buffer, 128);
    }
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

// writing an array of commands to the LCD
void LCD_writeCommand(INT8U* commands, INT8U length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DEVICE_CTRL_COMMAND, true);
    i2c_master_write(cmd, commands, length, true);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
}

// writing an array of data to the LCD
void LCD_writeData(INT8U* data, INT8U length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DEVICE_CTRL_DATA, true);
    i2c_master_write(cmd, data, length, true);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
}

// setting the coordinates to start writing on LCD
void LCD_setCoord(INT8U page, INT8U col) {
    INT8U cmd[3] = {(0xB0 + page), (col & 0x0F), (0x10 | ((col >> 4) & 0x0F))};
    LCD_writeCommand(cmd, 3);
}