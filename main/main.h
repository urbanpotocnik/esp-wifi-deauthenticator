#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "driver/uart.h"
#include "driver/gpio.h" 
void initialize_console(void);
esp_err_t wifi_init(void);
wifi_ap_record_t* scan_wifi_networks(uint16_t* ap_count);
void print_networks(wifi_ap_record_t* ap_records, uint16_t ap_count);
int get_user_selection(uint16_t max_networks);
esp_err_t send_deauth_frame(const wifi_ap_record_t *ap_record);
void wifi_deauth_control(void);

#endif 