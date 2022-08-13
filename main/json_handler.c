#include "main.h"
#include "used_defines.h"
#include "used_functions.h"

void jsonmanager(char* json_data, int json_data_length)  //Handles JSON commands
{

// cJSON parser variables //
cJSON* root = NULL; 
cJSON* led_entry = NULL;
cJSON* json_name = NULL;
cJSON* json_writing = NULL;
cJSON* json_reading = NULL;
cJSON* skill_name = NULL;
cJSON* connection_err = NULL;


char* json_teststring = NULL;
int json_number = NULL;
char* tarolohely[32];  //Store array data
char* got_name;

root = cJSON_ParseWithLength(json_data,json_data_length);

            json_number = cJSON_GetObjectItem(root,"number")->valueint;
            led_entry = cJSON_GetObjectItem(root,"access");
            json_reading = cJSON_GetObjectItem(root,"spiff_read");
            json_writing = cJSON_GetObjectItem(root,"spiff_write");
            json_name = cJSON_GetObjectItem(root,"name");
            got_name = cJSON_GetStringValue(json_name);
           
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
                Saving_names(got_name); 
              }  
              
            if (cJSON_IsTrue(json_reading)==true)
            {
               Reading_saved_names(); 
            }
            else{
                printf("Nothing to do...");
            }               
}