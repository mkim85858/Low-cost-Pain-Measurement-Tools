/**
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
#include <string.h>

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_spp_api.h"
#include "esp_gap_bt_api.h"

#include "Globals.h"
#include "HardwareConfig.h"
#include "Bluetooth.h"
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
#define SPP_TAG "BT_SPP"
#define DEVICE_NAME "ESP32_SPP_SERVER"
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
static BOOLEAN is_connected = false;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
static void Bluetooth_sppcb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Bluetooth Init
* @param    none
* @return   none
* @remark   Used to initialize bluetooth
********************************************************************************
*/
void Bluetooth_Init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_spp_register_callback(Bluetooth_sppcb));

    esp_spp_cfg_t bt_spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = true,
        .tx_buffer_size = 0
    };
    ESP_ERROR_CHECK(esp_spp_enhanced_init(&bt_spp_cfg));
}
/**
********************************************************************************
* @brief    Bluetooth Send to PC
* @param    data : pointer to data to send to PC
            len : length of data
* @return   none
* @remark   Used to initialize bluetooth
********************************************************************************
*/
void Bluetooth_SendToPC(INT16U *data, INT16U len) {
    // Check if there's an active connection
    if (is_connected) {
        ESP_LOGI(SPP_TAG, "Sending data to PC...");
        esp_err_t ret = esp_spp_write(129, len, data);
        if (ret == ESP_OK) {
            ESP_LOGI(SPP_TAG, "Data sent successfully");
        } 
        else {
            ESP_LOGE(SPP_TAG, "Failed to send data, error: %d", ret);
        }
    } 
    else {
        ESP_LOGW(SPP_TAG, "No active connection, can't send data");
    }
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
// Lowkey don't know what this function does but it's important nonetheless
static void Bluetooth_sppcb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_INIT_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
            esp_bt_gap_set_device_name(DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, "SPP_SERVER");
            break;

        case ESP_SPP_START_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
            break;

        case ESP_SPP_DATA_IND_EVT:
            ESP_LOGI(SPP_TAG, "Received data:");
            ESP_LOG_BUFFER_HEXDUMP(SPP_TAG, param->data_ind.data, param->data_ind.len, ESP_LOG_INFO);
            break;

        case ESP_SPP_SRV_OPEN_EVT:
            is_connected = true;
            ESP_LOGI(SPP_TAG, "Client connected");
            break;
        case ESP_SPP_CLOSE_EVT:
            is_connected = false;
            ESP_LOGI(SPP_TAG, "Client disconnected");
            break;
        default:
            break;
    }
}