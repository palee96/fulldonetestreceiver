void mqtt_app_start(void);
void jsonmanager(char* json_data, int json_data_length);
void Saving_names(char* names_to_save);
void Reading_saved_names();
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

void read_data_from_nvs();
void start_wifi_with_nvs();
esp_err_t psw_ssid_get_handler(httpd_req_t *req);
esp_err_t servePage_get_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void wifi_scan(void);
void event_handler_err(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
void event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
                                    
void wifi_init_softap(void);
char* json_send();