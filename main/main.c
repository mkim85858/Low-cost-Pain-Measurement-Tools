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
static INT16U maxPressure = 0;

static SemaphoreHandle_t semaphoreHandle = NULL;
static BOOLEAN taskActive = false;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void buttonTask(void* arg);
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
    OLED_WaitingForConnection();
    basePressure = Sensor_ReadPressure();
    maxPressure = basePressure + 1000;
    Bluetooth_Init();
    semaphoreHandle = xSemaphoreCreateBinary();
    Button_Init(buttonInterrupt);
    vTaskDelay(pdMS_TO_TICKS(10));
    OLED_WriteLogo();

    xTaskCreate(buttonTask, "buttonTask", 4096, NULL, 10, NULL);
}

// Task that waits reads pressure data and sends it to PC via Bluetooth
void buttonTask(void* arg) {
    while (1) {
        // When the task starts (starts reading), flush all other semaphores and clear OLED.
        if (xSemaphoreTake(semaphoreHandle, portMAX_DELAY) == pdTRUE) {
            taskActive = true;
            while (xSemaphoreTake(semaphoreHandle, 0) == pdTRUE) {}
            OLED_ClearScreen();
        }
        // Read pressure and send via bluetooth every 0.05 seconds
        while (taskActive) {
            INT16U pressure = Sensor_ReadPressure();
            Bluetooth_SendPressureTime(pressure, time);
            // Calculating how many progress bars to draw
            for (int i = 0; i < 10; i++) {
                if ((pressure - basePressure) * 10 > (i * (maxPressure - basePressure))) {
                    OLED_WriteProgressBar(1, (12 * i) + 4, 1);
                }
                else {
                    OLED_WriteProgressBar(1, (12 * i) + 4, 0);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(50));         // Send data every 0.05 seconds (for now)
            // When the task ends (stops reading), write logo and send 0s via bluetooth to signal end
            if (xSemaphoreTake(semaphoreHandle, 0) == pdTRUE) {
                OLED_WriteLogo();
                taskActive = false;
                Bluetooth_SendPressureTime(0, 0);
                time = 0;
                break;
            }
            time++;
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