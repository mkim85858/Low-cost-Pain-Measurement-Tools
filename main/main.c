/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "Common/I2CDrv.c"
#include "PSensor/PSensorDrv.h"
#include "OLED/OLEDDrv.h"
#include "Bluetooth/Bluetooth.h"
#include "Button/ButtonDrv.h"
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
static INT8U time = 0;
static INT64U lastInterruptTime = 0;
static INT16U basePressure = 0;
static INT16U maxIncrease = 0;

static SemaphoreHandle_t semaphoreHandle = NULL;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void mainTask(void* arg);
static void buttonInterrupt(void* arg);
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
    OLED_Init();
    basePressure = Sensor_ReadPressure();
    maxIncrease = 1000;     // arbitrarily set value
    Bluetooth_Init();
    semaphoreHandle = xSemaphoreCreateBinary();
    Button_Init(buttonInterrupt);
    vTaskDelay(pdMS_TO_TICKS(10));
    OLED_WriteLogo();

    xTaskCreate(mainTask, "mainTask", 4096, NULL, 10, NULL);
}

// Task that waits reads pressure data and sends it to PC via Bluetooth
void mainTask(void* arg) {
    while (1) {
        // When the task starts (start reading), flush all other semaphores and clear OLED
        if (xSemaphoreTake(semaphoreHandle, portMAX_DELAY) == pdTRUE) {
            while (xSemaphoreTake(semaphoreHandle, 0) == pdTRUE) {}
            OLED_ClearScreen();
            // Read pressure and send via bluetooth every 0.05 seconds
            while (1) {
                INT16U pressure = Sensor_ReadPressure();
                Bluetooth_SendPressureTime(pressure, time);
                // Drawing progress bars
                for (INT8U i = 0; i < 10; i++) {
                    OLED_WriteProgressBar(1, (12 * i) + 4, (pressure - basePressure) * 10 > (maxIncrease * i));
                }
                vTaskDelay(pdMS_TO_TICKS(50));
                // When the task ends (stop reading), write logo and send 0s via bluetooth to signal end
                if (xSemaphoreTake(semaphoreHandle, 0) == pdTRUE) {
                    OLED_WriteLogo();
                    Bluetooth_SendPressureTime(0, 0);
                    time = 0;
                    break;
                }
            }
        }
    }
}

// Interrupt for when the button is released
void buttonInterrupt(void *arg) {
    INT64U now = esp_timer_get_time();
    if (now - lastInterruptTime > 100000) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(semaphoreHandle, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
        lastInterruptTime = now;
    }
}