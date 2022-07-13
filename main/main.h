#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "sys/unistd.h"
#include "sys/stat.h"
#include "mbedtls/md5.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "wifi_manager.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
