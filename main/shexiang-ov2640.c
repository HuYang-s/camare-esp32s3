#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_timer.h"

#include "esp_camera.h"

// WiFi credentials from Kconfig
#define WIFI_SSID       CONFIG_WIFI_SSID
#define WIFI_PASS       CONFIG_WIFI_PASSWORD

static const char *TAG = "ov2640_app";

// WiFi event group flags
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t wifi_event_group;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		ESP_LOGW(TAG, "WiFi disconnected, retrying...");
		esp_wifi_connect();
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
		xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

static void wifi_init_sta(void)
{
	esp_netif_init();
	esp_event_loop_create_default();
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	wifi_config_t wifi_config = { 0 };
	strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
	strncpy((char *)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.pmf_cfg.capable = true;
	wifi_config.sta.pmf_cfg.required = false;

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
	esp_wifi_start();

	ESP_LOGI(TAG, "WiFi init finished. Connecting to SSID: %s", WIFI_SSID);
}

static esp_err_t init_camera(void)
{
	camera_config_t config = {
		.ledc_channel = LEDC_CHANNEL_0,
		.ledc_timer = LEDC_TIMER_0,
		.pin_d0 = CONFIG_CAMERA_PIN_D0,
		.pin_d1 = CONFIG_CAMERA_PIN_D1,
		.pin_d2 = CONFIG_CAMERA_PIN_D2,
		.pin_d3 = CONFIG_CAMERA_PIN_D3,
		.pin_d4 = CONFIG_CAMERA_PIN_D4,
		.pin_d5 = CONFIG_CAMERA_PIN_D5,
		.pin_d6 = CONFIG_CAMERA_PIN_D6,
		.pin_d7 = CONFIG_CAMERA_PIN_D7,
		.pin_xclk = CONFIG_CAMERA_PIN_XCLK,
		.pin_pclk = CONFIG_CAMERA_PIN_PCLK,
		.pin_vsync = CONFIG_CAMERA_PIN_VSYNC,
		.pin_href = CONFIG_CAMERA_PIN_HREF,
		.pin_sccb_sda = CONFIG_CAMERA_PIN_SIOD,
		.pin_sccb_scl = CONFIG_CAMERA_PIN_SIOC,
		.pin_pwdn = CONFIG_CAMERA_PIN_PWDN,
		.pin_reset = CONFIG_CAMERA_PIN_RESET,
		.xclk_freq_hz = CONFIG_CAMERA_XCLK_FREQ,
		.pixel_format = PIXFORMAT_JPEG,
		.frame_size = FRAMESIZE_VGA,
		.jpeg_quality = CONFIG_CAMERA_JPEG_QUALITY,
		.fb_count = CONFIG_CAMERA_FRAMEBUFFER_COUNT,
		.fb_location = CAMERA_FB_IN_PSRAM,
		.grab_mode = CAMERA_GRAB_WHEN_EMPTY
	};

	// If no PSRAM, reduce buffers and place in DRAM
	#ifdef CONFIG_SPIRAM
		config.fb_location = CAMERA_FB_IN_PSRAM;
	#else
		config.fb_location = CAMERA_FB_IN_DRAM;
		config.fb_count = 1;
		config.frame_size = FRAMESIZE_QVGA;
		ESP_LOGW(TAG, "PSRAM not enabled. Using QVGA and single framebuffer");
	#endif

	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
		return err;
	}

	// Optionally set some sensor params
	sensor_t *s = esp_camera_sensor_get();
	if (s)
	{
		// Flip or adjust if image orientation is wrong
		// s->set_vflip(s, 1);
		// s->set_hmirror(s, 1);
	}

	return ESP_OK;
}

// HTTP server and handlers
static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
static const char *STREAM_BOUNDARY = "--frame";
static const char *STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t index_handler(httpd_req_t *req)
{
	static const char html[] =
		"<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>ESP32-S3 OV2640</title>"
		"<style>body{font-family:sans-serif;margin:20px;}img{max-width:100%;height:auto;}</style>"
		"</head><body><h2>ESP32-S3 OV2640 Stream</h2>"
		"<p><img src=\"/stream\"/></p>"
		"<p><a href=\"/jpg\">Capture JPG</a></p>"
		"</body></html>";

	httpd_resp_set_type(req, "text/html");
	return httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t jpg_handler(httpd_req_t *req)
{
	camera_fb_t *fb = esp_camera_fb_get();
	if (!fb)
	{
		ESP_LOGE(TAG, "Camera capture failed");
		return httpd_resp_send_500(req);
	}

	httpd_resp_set_type(req, "image/jpeg");
	esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
	esp_camera_fb_return(fb);
	return res;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
	esp_err_t res = ESP_OK;
	char part_buf[64];

	httpd_resp_set_type(req, STREAM_CONTENT_TYPE);

	while (true)
	{
		camera_fb_t *fb = esp_camera_fb_get();
		if (!fb)
		{
			ESP_LOGE(TAG, "Camera capture failed");
			res = ESP_FAIL;
			break;
		}

		int hlen = snprintf(part_buf, sizeof(part_buf), STREAM_PART, fb->len);
		if ((res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY))) != ESP_OK)
		{
			esp_camera_fb_return(fb);
			break;
		}
		if ((res = httpd_resp_send_chunk(req, part_buf, hlen)) != ESP_OK)
		{
			esp_camera_fb_return(fb);
			break;
		}
		if ((res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len)) != ESP_OK)
		{
			esp_camera_fb_return(fb);
			break;
		}
		if ((res = httpd_resp_send_chunk(req, "\r\n", 2)) != ESP_OK)
		{
			esp_camera_fb_return(fb);
			break;
		}

		esp_camera_fb_return(fb);
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	// End the stream if error
	httpd_resp_send_chunk(req, NULL, 0);
	return res;
}

static httpd_handle_t start_webserver(void)
{
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.server_port = 80;
	config.uri_match_fn = httpd_uri_match_wildcard;

	httpd_handle_t server = NULL;
	if (httpd_start(&server, &config) == ESP_OK)
	{
		httpd_uri_t index_uri = {
			.uri = "/",
			.method = HTTP_GET,
			.handler = index_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &index_uri);

		httpd_uri_t jpg_uri = {
			.uri = "/jpg",
			.method = HTTP_GET,
			.handler = jpg_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &jpg_uri);

		httpd_uri_t stream_uri = {
			.uri = "/stream",
			.method = HTTP_GET,
			.handler = stream_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &stream_uri);
	}

	return server;
}

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	wifi_event_group = xEventGroupCreate();
	wifi_init_sta();

	EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(15000));
	if ((bits & WIFI_CONNECTED_BIT) == 0)
	{
		ESP_LOGE(TAG, "WiFi connection timed out");
		// Continue anyway; user may connect later
	}

	ESP_ERROR_CHECK(init_camera());

	start_webserver();

	ESP_LOGI(TAG, "Setup complete. Open http://<device-ip>/ in your browser.");
}
