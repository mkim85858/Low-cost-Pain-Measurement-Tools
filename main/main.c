/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "PSensor/PSensorDrv.h"
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
/* Insert static function prototypes here */
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

void app_main(void) {
    I2C_Init();
    Sensor_Init();
    INT8U buf[3] = {0x00, 0x00, 0x00};
    Sensor_ReadPressure(buf);
    ESP_LOGI("MAIN", "0x%02x", buf[0]);
    ESP_LOGI("MAIN", "0x%02x", buf[1]);
    ESP_LOGI("MAIN", "0x%02x", buf[2]);
    int total = (buf[0] << 16) | (buf[1] << 8) | buf[2];
    total /= 4096;
    ESP_LOGI("MAIN", "Current atmospheric pressure is %d!", total);
    ESP_LOGI("MAIN", "task finished...");
}