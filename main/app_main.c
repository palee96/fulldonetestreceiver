#include "main.h"

// MQTT broker, topic info
#define BROKER_URL ""
#define IO_TOPIC ""
static const char *TAG = "Main";

//Adafruit IO certificate 
extern const uint8_t adafruitmqtts_pem_start[]   asm("_binary_adafruitmqtts_pem_start");
extern const uint8_t adafruitmqtts_pem_end[]   asm("_binary_adafruitmqtts_pem_end");


void Booting_Spiffs(){

esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

esp_err_t ret = esp_vfs_spiffs_register(&conf);

if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }  
}

void Saving_to_nvs(char* names_to_save[32]){

   FILE* Saved_Names = fopen("/spiffs/names.txt", "w");
if (Saved_Names == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

fprintf(Saved_Names,names_to_save); 
fclose(Saved_Names); 
}

void Reading_from_nvs(int szamlalo_array){
FILE* Saved_Names_v2 = fopen("/spiffs/names.txt", "r");
if (Saved_Names_v2 == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
char *line[32];
fgets(line, sizeof(line), Saved_Names_v2);
fclose(Saved_Names_v2);

for (size_t i = 0; i < szamlalo_array; i++)
{
    ESP_LOGI(TAG, "\nRead from file: %s\n", line[i]);
}
}

// WiFi-Manager
void cb_connection_ok(void *pvParameter){
	ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

	/* transform IP to human readable string */
	char str_ip[16];
	esp_ip4addr_ntoa(&param->ip_info.ip, str_ip, IP4ADDR_STRLEN_MAX);

	ESP_LOGI(TAG, "I have a connection and my IP is %s!", str_ip);
}

void jsonmanager(char* json_data, int json_data_length)  //Handles JSON commands
{

// cJSON parser variables //
cJSON* root = NULL; 
cJSON* led_entry = NULL;
cJSON* json_name = NULL;
cJSON* json_writing = NULL;
cJSON* json_reading = NULL;
cJSON* skill_name = NULL;
cJSON* parse_skill_name = NULL;

char* json_teststring = NULL;
int json_number = NULL;
char* tarolohely[32];  //Store array data


root = cJSON_ParseWithLength(json_data,json_data_length);

            json_number = cJSON_GetObjectItem(root,"number")->valueint;
            led_entry = cJSON_GetObjectItem(root,"access");
            json_reading = cJSON_GetObjectItem(root,"spiff_read");
            json_writing = cJSON_GetObjectItem(root,"spiff_write");
            json_name = cJSON_GetObjectItem(root,"name");

            skill_name = cJSON_GetObjectItem(root,"skill_name"); // cJSON array

            int skill_name_length = cJSON_GetArraySize(skill_name);
            
            for (size_t i = 0; i < skill_name_length; i++)
            {
                parse_skill_name = cJSON_GetArrayItem(skill_name,i);
                tarolohely[i] = cJSON_GetStringValue(parse_skill_name);
            }
            
            if (cJSON_IsTrue(led_entry)==true)
            {
              gpio_set_direction(json_number, GPIO_MODE_OUTPUT);  
              gpio_set_level(json_number, 1);
              printf("The LED is turned on\n");
              vTaskDelay(400);
              gpio_set_level(json_number,0);
              printf("The LED is turned off\n");
            }
            if (cJSON_IsTrue(json_writing)==true)
              {
                Saving_to_nvs(tarolohely); // elmentett_nevek
              }  
              
            if (cJSON_IsTrue(json_reading)==true)
            {
               Reading_from_nvs(skill_name_length);
            }
            else{
                printf("Nothing to do...");
            }         
}


int mqtt_connected = 0;

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, IO_TOPIC, 0);
            mqtt_connected = 1;
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_connected = 0;
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            jsonmanager(event->data, event->data_len);  // Sends data from MQTT to process
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
           
    return ESP_OK;
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = BROKER_URL,
        .event_handle = mqtt_event_handler,
        .cert_pem = (const char*)adafruitmqtts_pem_start,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

}


void app_main()
{
    
    nvs_flash_init();
    wifi_manager_start();
    wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &cb_connection_ok);
    Booting_Spiffs();
    mqtt_app_start();
      
}
