#include "main.h"
#include "used_defines.h"
#include "used_functions.h"
//#include "extern_veriables.h"
static char __SSID[64];
 static char __PWD[32];

//Start wifi with saved SSID and PWD 

void start_wifi_with_nvs(){

esp_wifi_set_mode(WIFI_MODE_STA);


    wifi_config_t wifi_internet_config = {};
        strcpy((char*)wifi_internet_config.sta.ssid, __SSID);
        strcpy((char*)wifi_internet_config.sta.password, __PWD);
     
esp_wifi_set_config(WIFI_IF_STA,&wifi_internet_config);
esp_wifi_start();
esp_wifi_connect();

}

// Read SSID AND PWD from NVS

 void read_data_from_nvs(){


 nvs_handle_t my_handle;
 size_t required_size_pwd;
 size_t required_size_ssid; 
       esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {
            

            
        nvs_get_str(my_handle, "__SSID", NULL, &required_size_ssid);
        
        nvs_get_str(my_handle, "__SSID", __SSID, &required_size_ssid);


        nvs_get_str(my_handle, "__PWD", NULL, &required_size_pwd);
        
        nvs_get_str(my_handle, "__PWD", __PWD, &required_size_pwd);

        }
        // Close
        nvs_close(my_handle);
        printf("EZ VAN A PWD_NAMEBEN: %s\n",__PWD);
        printf("EZ VAN A SSID_NAMEBEN: %s\n",__SSID);
}


/* Initialize Wi-Fi as sta and set scan method */
 void wifi_scan(void)
{
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    esp_wifi_scan_start(NULL, true);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);

    bool stop = false;
    bool stop_ap = false;
    
    char tarhely[32];
    strcpy(tarhely,__SSID);
    printf("Ez van a tarhelyben: %s\n",tarhely);
    if (strcmp(tarhely,"")==0)
    {
        printf("No SSID has been saved...starting soft-AP!\n");
        stop = true;
        wifi_init_softap();
    }
    else{
        for (size_t j = 0; (j < maximum_retries ) && !stop ; j++)
    {
        for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count) && !stop; i++) {
        
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        vTaskDelay(10);
        
        if (strstr((char*)ap_info[i].ssid,tarhely)!=NULL)
        {
            printf("Found saved SSID!\n");
            stop = true;
            stop_ap = true;
            start_wifi_with_nvs();
            break;
        }
        else if (strstr((char*)ap_info[i].ssid,tarhely)==NULL)  
        {
            printf("Continueing search....\n");
            continue;
        }
        
    }
   
    printf("kiment a forbol\n");
    }
    if (stop_ap==true)
    {
        printf("...\n");
    }
    else{
    printf("Didn't find saved SSID, starting soft-AP...\n");
    wifi_init_softap();
    }
      
}
    }
    
    
    