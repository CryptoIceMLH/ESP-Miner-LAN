#include "esp_event.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "nvs_flash.h"

#include "asic_result_task.h"
#include "asic_task.h"
#include "create_jobs_task.h"
#include "statistics_task.h"
#include "system.h"
#include "http_server.h"
#include "serial.h"
#include "stratum_task.h"
#include "i2c_bitaxe.h"
#include "adc.h"
#include "nvs_device.h"
#include "nvs_config.h"
#include "self_test.h"
#include "asic.h"
#include "bap/bap.h"
#include "device_config.h"
#include "connect.h"
#include "asic_reset.h"

static GlobalState GLOBAL_STATE;

static const char * TAG = "bitaxe";

void app_main(void)
{
    ESP_LOGI(TAG, "Welcome to the bitaxe - FOSS || GTFO!");

    if (!esp_psram_is_initialized()) {
        ESP_LOGE(TAG, "No PSRAM available on ESP32 device!");
        GLOBAL_STATE.psram_is_available = false;
    } else {
        GLOBAL_STATE.psram_is_available = true;
    }

    // Init I2C
    ESP_ERROR_CHECK(i2c_bitaxe_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    //wait for I2C to init
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //Init ADC
    ADC_init();

    //initialize the ESP32 NVS
    if (NVSDevice_init() != ESP_OK){
        ESP_LOGE(TAG, "Failed to init NVS");
        return;
    }

    if (device_config_init(&GLOBAL_STATE) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init device config");
        return;
    }

    if (self_test(&GLOBAL_STATE)) return;

    SYSTEM_init_system(&GLOBAL_STATE);
    statistics_init(&GLOBAL_STATE);

    // Initialize network infrastructure ONCE before any interface init
    network_infrastructure_init();

    // Read network mode to determine which interface to initialize
    char *network_mode_str = nvs_config_get_string(NVS_CONFIG_NETWORK_MODE, "wifi");
    bool use_ethernet = (strcmp(network_mode_str, "ethernet") == 0);
    free(network_mode_str);

    if (use_ethernet) {
        ESP_LOGI(TAG, "Network mode: Ethernet - Initializing...");
        // Try to init Ethernet
        ethernet_init(&GLOBAL_STATE);
        ESP_LOGI(TAG, "DEBUG: After ethernet_init, eth_available = %d", GLOBAL_STATE.ETHERNET_MODULE.eth_available);

        // Wait for Ethernet to get IP and update is_connected flag
        if (GLOBAL_STATE.ETHERNET_MODULE.eth_available) {
            ESP_LOGI(TAG, "Waiting for Ethernet IP address...");
            int retry_count = 0;
            while (retry_count < 100) {  // Wait up to 10 seconds
                ethernet_update_status(&GLOBAL_STATE);
                if (GLOBAL_STATE.SYSTEM_MODULE.is_connected) {
                    ESP_LOGI(TAG, "Ethernet connected with IP: %s", GLOBAL_STATE.ETHERNET_MODULE.eth_ip_addr_str);
                    break;
                }
                vTaskDelay(100 / portTICK_PERIOD_MS);
                retry_count++;
            }
            if (!GLOBAL_STATE.SYSTEM_MODULE.is_connected) {
                ESP_LOGW(TAG, "Ethernet timeout, falling back to WiFi");
                wifi_init(&GLOBAL_STATE);
            }
        } else {
            ESP_LOGW(TAG, "Ethernet unavailable, initializing WiFi fallback");
            wifi_init(&GLOBAL_STATE);
        }
    } else {        ESP_LOGI(TAG, "Network mode: WiFi");
        // init AP and connect to wifi
        wifi_init(&GLOBAL_STATE);
        // init Ethernet detection (but not full init)
        ethernet_init(&GLOBAL_STATE);
        ESP_LOGI(TAG, "DEBUG: After ethernet_init, eth_available = %d", GLOBAL_STATE.ETHERNET_MODULE.eth_available);
    }

    SYSTEM_init_peripherals(&GLOBAL_STATE);

    xTaskCreate(POWER_MANAGEMENT_task, "power management", 8192, (void *) &GLOBAL_STATE, 10, NULL);

    //start the API for AxeOS
    start_rest_server((void *) &GLOBAL_STATE);

#ifdef CONFIG_ENABLE_BAP
    // Initialize BAP interface
    esp_err_t bap_ret = BAP_init(&GLOBAL_STATE);
    if (bap_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BAP interface: %d", bap_ret);
        // Continue anyway, as BAP is not critical for core functionality
    } else {
        ESP_LOGI(TAG, "BAP interface initialized successfully");
    }
#endif

    while (!GLOBAL_STATE.SYSTEM_MODULE.is_connected) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    queue_init(&GLOBAL_STATE.stratum_queue);
    queue_init(&GLOBAL_STATE.ASIC_jobs_queue);

    if (asic_reset() != ESP_OK) {
        GLOBAL_STATE.SYSTEM_MODULE.asic_status = "ASIC reset failed";
        ESP_LOGE(TAG, "ASIC reset failed!");
        return;
    }

    SERIAL_init();

    if (ASIC_init(&GLOBAL_STATE) == 0) {
        GLOBAL_STATE.SYSTEM_MODULE.asic_status = "Chip count 0";
        ESP_LOGE(TAG, "Chip count 0");
        return;
    }

    SERIAL_set_baud(ASIC_set_max_baud(&GLOBAL_STATE));
    SERIAL_clear_buffer();

    GLOBAL_STATE.ASIC_initalized = true;

    xTaskCreate(stratum_task, "stratum admin", 8192, (void *) &GLOBAL_STATE, 5, NULL);
    xTaskCreate(create_jobs_task, "stratum miner", 8192, (void *) &GLOBAL_STATE, 10, NULL);
    xTaskCreate(ASIC_task, "asic", 8192, (void *) &GLOBAL_STATE, 10, NULL);
    xTaskCreate(ASIC_result_task, "asic result", 8192, (void *) &GLOBAL_STATE, 15, NULL);
    xTaskCreate(statistics_task, "statistics", 8192, (void *) &GLOBAL_STATE, 3, NULL);
}
