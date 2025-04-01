/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "nvs_flash.h"

#include "Common/I2CDrv.c"
#include "PSensor/PSensorDrv.h"
#include "LCD/LCDDrv.h"
#include "Bluetooth/Bluetooth.h"
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
void esp_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_OPEN_EVT:
            ESP_LOGI("BT", "Client Connected!");
            break;
        case ESP_SPP_DATA_IND_EVT:
            ESP_LOGI("BT", "Received data: %.*s", param->data_ind.len, param->data_ind.data);
            break;
        default:
            break;
    }
}

void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    if (event == ESP_BT_GAP_AUTH_CMPL_EVT) {
        ESP_LOGI("BT", "Authentication success: %s", param->auth_cmpl.device_name);
    }
}


void app_main(void) {
    I2C_Init();
    //Bluetooth_Init();
    Sensor_Init();
    LCD_Init();


    ESP_LOGI("MAIN", "Current atmospheric pressure is %d!", Sensor_ReadPressure());
    



    esp_err_t ret;
    nvs_flash_init();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    ESP_ERROR_CHECK(ret);
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    ESP_ERROR_CHECK(ret);

    ret = esp_bluedroid_init();
    ESP_ERROR_CHECK(ret);
    ret = esp_bluedroid_enable();
    ESP_ERROR_CHECK(ret);

    ret = esp_bt_gap_register_callback(bt_app_gap_cb);
    ESP_ERROR_CHECK(ret);

    ret = esp_spp_register_callback(esp_spp_callback);
    ESP_ERROR_CHECK(ret);
    
    ret = esp_spp_init(ESP_SPP_MODE_CB);
    ESP_ERROR_CHECK(ret);

    ret = esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0, "ESP32_SPP_SERVER");
    ESP_ERROR_CHECK(ret);

    ESP_LOGI("MAIN", "Bluetooth SPP Initialized.");
}