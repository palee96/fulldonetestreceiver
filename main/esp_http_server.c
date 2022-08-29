#include "main.h"
#include "used_defines.h"
#include "used_functions.h"


static char __SSID[64];
static char __PWD[32];

// WEBSERVER INITIALIZATION

/* An HTTP POST handler */
 esp_err_t psw_ssid_get_handler(httpd_req_t *req)
{
    char buf[128];
    int ret, remaining = req->content_len;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == 0)
            {
                ESP_LOGI(TAG, "No content received please try again ...");
            }
            else if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {

                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "===================================="); 
        cJSON *root = cJSON_Parse(buf);

        sprintf(__SSID, "%s", cJSON_GetObjectItem(root, "ssid")->valuestring);
        sprintf(__PWD, "%s", cJSON_GetObjectItem(root, "pwd")->valuestring);

        ESP_LOGI(TAG, "pwd: %s", __PWD);
        ESP_LOGI(TAG, "ssid: %s", __SSID);

        remaining -= ret;

        nvs_handle_t my_handle;
    
       esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {
            printf("Done\n");
            err = nvs_set_str(my_handle, "__PWD", __PWD);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            err = nvs_set_str(my_handle,"__SSID",__SSID);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            err = nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
        }
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);

    //start_wifi_with_nvs();
    wifi_scan();
    return ESP_OK;
}



 const httpd_uri_t psw_ssid = {
    .uri       = "/connection2",
    .method    = HTTP_POST,
    .handler   = psw_ssid_get_handler,
    .user_ctx  = "TEST",
};


/* An HTTP GET handler */
 esp_err_t servePage_get_handler(httpd_req_t *req)
{
  httpd_resp_sendstr_chunk(req, "<!DOCTYPE html><html>");

    httpd_resp_sendstr_chunk(req, "<head>");
    httpd_resp_sendstr_chunk(req, "<style>");
    httpd_resp_sendstr_chunk(req, "form {display: grid;padding: 1em; background: #f9f9f9; border: 1px solid #c1c1c1; margin: 2rem auto 0 auto; max-width: 400px; padding: 1em;}}");
    httpd_resp_sendstr_chunk(req, "form input {background: #fff;border: 10px solid #9c9c9c;}");
    httpd_resp_sendstr_chunk(req, "form button {background: lightgrey; padding: 0.7em;width: 100%; border: 0;");
    httpd_resp_sendstr_chunk(req, "label {padding: 0.5em 0.5em 0.5em 0;}");
    httpd_resp_sendstr_chunk(req, "input {padding: 0.7em;margin-bottom: 0.5rem;}");
    httpd_resp_sendstr_chunk(req, "input:focus {outline: 10px solid gold;}");
    httpd_resp_sendstr_chunk(req, "@media (min-width: 300px) {form {grid-template-columns: 200px 1fr; grid-gap: 16px;} label { text-align: right; grid-column: 1 / 2; } input, button { grid-column: 2 / 3; }}");
    httpd_resp_sendstr_chunk(req, "</style>");
    httpd_resp_sendstr_chunk(req, "</head>");

    httpd_resp_sendstr_chunk(req, "<body>");
    httpd_resp_sendstr_chunk(req, "<form class=\"form1\" id=\"loginForm\" action=\"\">");

    httpd_resp_sendstr_chunk(req, "<label for=\"SSID\">WiFi Name</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"ssid\" type=\"text\" name=\"ssid\" maxlength=\"64\" minlength=\"4\" required>");

    httpd_resp_sendstr_chunk(req, "<label for=\"Password\">Password</label>");
    httpd_resp_sendstr_chunk(req, "<input id=\"pwd\" type=\"password\" name=\"pwd\" maxlength=\"64\" minlength=\"4\" required>");

    httpd_resp_sendstr_chunk(req, "<button tpye = \"submit\">Submit</button>");
    httpd_resp_sendstr_chunk(req, "</form>");

    httpd_resp_sendstr_chunk(req, "<script>");  
    httpd_resp_sendstr_chunk(req, "document.getElementById(\"loginForm\").addEventListener(\"submit\", (e) => {e.preventDefault(); const formData = new FormData(e.target);  const data = Object.fromEntries(formData); console.log(JSON.stringify(data)); var xhr = new XMLHttpRequest(); xhr.open(\"POST\", \"http://192.168.4.1/connection2\", true); xhr.setRequestHeader('Content-Type', 'application/json'); xhr.send(JSON.stringify(data));});");
    httpd_resp_sendstr_chunk(req, "</script>");

    httpd_resp_sendstr_chunk(req, "</body></html>");

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


 const httpd_uri_t servePage = {
    .uri       = "/wifiap2",
    .method    = HTTP_GET,
    .handler   = servePage_get_handler,
    .user_ctx  = NULL,
};

 httpd_handle_t start_webserver(void)
{
    
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &servePage);
        httpd_register_uri_handler(server, &psw_ssid);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}