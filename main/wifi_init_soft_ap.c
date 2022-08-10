#include "main.h"
#include "used_defines.h"
#include "used_functions.h"
//#include "extern_veriables.h"

// SOFT AP INITIALIZATION

void wifi_init_softap(void){

/*Initialization of protocols and default settings*/
 
esp_netif_init();  //Starts underlying TCP/IP

esp_event_loop_create_default();  //Start default event loop, that is a special type of loop used for system events (Wi-Fi events, for example).

esp_netif_create_default_wifi_ap();  //Create default API

wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //Initialize wifi with default config
esp_wifi_init(&cfg);


// Setting specific setting for AP,WiFi,etc.
wifi_config_t wifi_config = {
        .ap = {
            .ssid = esp_wifi_id,
            .ssid_len = strlen(esp_wifi_id),
            .password = esp_wifi_pass,
            .max_connection = 5,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
};

esp_wifi_set_mode(WIFI_MODE_AP);
esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
esp_wifi_start();

ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             esp_wifi_id, esp_wifi_pass);
}