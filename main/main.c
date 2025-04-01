/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Common/I2CDrv.c"
#include "PSensor/PSensorDrv.h"
#include "LCD/LCDDrv.h"
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
    LCD_Init();
    ESP_LOGI("MAIN", "Current atmospheric pressure is %d!", Sensor_ReadPressure());
    
    INT8U i = 0;
    while(true) {
        LCD_WriteProgressBar(3, (12 * i) + 4, 1);
        i = i + 1;
        vTaskDelay(pdMS_TO_TICKS(250));
        if (i == 10) {
            i = 0;
            LCD_ClearPage(3);
            LCD_ClearPage(4);
        }
    }
}