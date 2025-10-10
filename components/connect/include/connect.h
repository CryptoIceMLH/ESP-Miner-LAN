#ifndef CONNECT_H_
#define CONNECT_H_

#include "lwip/sys.h"
#include <arpa/inet.h>
#include <lwip/netdb.h>

#include "esp_wifi_types.h"
#include "global_state.h"

// Structure to hold WiFi scan results
typedef struct {
    char ssid[33];  // 32 chars + null terminator
    int8_t rssi;
    wifi_auth_mode_t authmode;
} wifi_ap_record_simple_t;

// WiFi functions
void toggle_wifi_softap(void);
void wifi_init(void * GLOBAL_STATE);
esp_err_t wifi_scan(wifi_ap_record_simple_t *ap_records, uint16_t *ap_count);
esp_err_t get_wifi_current_rssi(int8_t *rssi);

// Ethernet functions
void ethernet_init(GlobalState *state);
void ethernet_update_status(GlobalState *state);
esp_err_t switch_to_ethernet_mode(GlobalState *state);
esp_err_t switch_to_wifi_mode(GlobalState *state);

#endif /* CONNECT_H_ */
