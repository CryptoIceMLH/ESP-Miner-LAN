/**
 * @file ethernet_w5500.h
 * @brief W5500 Ethernet driver for ESP-Miner via SPI
 *
 * This component provides W5500 Ethernet connectivity as an alternative
 * to WiFi networking. Uses SPI interface via BAP port pins.
 */

#ifndef ETHERNET_W5500_H_
#define ETHERNET_W5500_H_

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize W5500 Ethernet hardware and network interface
 *
 * This function:
 * - Initializes SPI bus for W5500
 * - Detects W5500 hardware presence
 * - Configures MAC address from ESP32 chip ID
 * - Starts DHCP client or configures static IP
 * - Creates esp_netif for Ethernet
 *
 * @return ESP_OK on success
 *         ESP_ERR_NOT_FOUND if W5500 hardware not detected
 *         ESP_FAIL on other initialization errors
 */
esp_err_t ethernet_w5500_init(void);

/**
 * @brief Check if W5500 hardware was detected during initialization
 *
 * @return true if W5500 hardware is present and initialized
 *         false if hardware not found or initialization failed
 */
bool ethernet_w5500_is_available(void);

/**
 * @brief Check if Ethernet link is up and IP address assigned
 *
 * @return true if Ethernet cable connected and IP assigned
 *         false if link down or no IP address
 */
bool ethernet_w5500_is_connected(void);

/**
 * @brief Get current Ethernet IP address as string
 *
 * @param ip_str Buffer to store IP address string
 * @param len Buffer length (should be at least 16 bytes for IPv4)
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if ip_str is NULL
 *         ESP_ERR_INVALID_STATE if Ethernet not connected
 */
esp_err_t ethernet_w5500_get_ip(char *ip_str, size_t len);

/**
 * @brief Get Ethernet MAC address as string
 *
 * @param mac_str Buffer to store MAC address string (format: "AA:BB:CC:DD:EE:FF")
 * @param len Buffer length (should be at least 18 bytes)
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if mac_str is NULL
 *         ESP_ERR_INVALID_STATE if Ethernet not initialized
 */
esp_err_t ethernet_w5500_get_mac(char *mac_str, size_t len);

/**
 * @brief Check and update Ethernet link status
 *
 * Polls W5500 for link status changes. Call this periodically from main loop.
 */
void ethernet_w5500_check_link(void);

/**
 * @brief Get W5500 PHY link status (cable connected)
 *
 * @return true if Ethernet cable is physically connected (link up)
 *         false if cable disconnected or W5500 not initialized
 */
bool ethernet_w5500_get_link_status(void);

/**
 * @brief Stop Ethernet interface and release resources
 *
 * @return ESP_OK on success
 */
esp_err_t ethernet_w5500_stop(void);

/**
 * @brief Restart Ethernet interface with current configuration
 *
 * @return ESP_OK on success
 */
esp_err_t ethernet_w5500_restart(void);

#ifdef __cplusplus
}
#endif

#endif /* ETHERNET_W5500_H_ */
