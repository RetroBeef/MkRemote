#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "nvs_flash.h"
#include "esp_partition.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "mk.h"

static const char* LOG_TAG = "MKRemote";

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static esp_bd_addr_t rnd_addr = { 0x51 | 0b11000000, 0x02, 0x8a, 0x34, 0x79, 0xef };

uint8_t commandDataHello[] = {170, 3, 40, 128, 128, 0, 129,  85};

static esp_ble_adv_params_t ble_adv_params = {0};
static esp_ble_adv_data_t ble_adv_data = {0};

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  esp_err_t err;

  switch (event) {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
      esp_ble_gap_start_advertising(&ble_adv_params);
      break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
      if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
        ESP_LOGE(LOG_TAG, "advertising start failed: %s", esp_err_to_name(err));
      } else {
        ESP_LOGI(LOG_TAG, "advertising has started.");
      }
      break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
      if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
        ESP_LOGE(LOG_TAG, "adv stop failed: %s", esp_err_to_name(err));
      }
      else {
        ESP_LOGI(LOG_TAG, "stop adv successfully");
      }
      break;
    default:
      break;
  }
}

void advertiseMkData(uint8_t* commandData, size_t commandDataSize){
  esp_err_t status;
  uint8_t* mkAdvertisementData = getMkAdvertisementData(commandData, commandDataSize);
  if ((esp_ble_gap_config_adv_data_raw(mkAdvertisementData, getMkAdvertisementDataSize())) != ESP_OK) {
    ESP_LOGE(LOG_TAG, "couldn't configure BLE adv: %s", esp_err_to_name(status));
    return;
  }
}

void setMotors(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
  uint8_t commandData[]  = {102, 3, 40, a, b, c, d, 153};
  advertiseMkData(commandData, sizeof(commandData));
}

void setup() {
  Serial.begin(115200);
  
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);

  esp_bluedroid_init();
  esp_bluedroid_enable();

  ble_adv_params.adv_int_min       = 0x20;
  ble_adv_params.adv_int_max       = 0x40;
  ble_adv_params.adv_type          = ADV_TYPE_IND;
  ble_adv_params.own_addr_type     = BLE_ADDR_TYPE_RANDOM;
  ble_adv_params.channel_map       = ADV_CHNL_ALL;
  ble_adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
  ble_adv_params.peer_addr_type    = BLE_ADDR_TYPE_PUBLIC;


  ESP_LOGI(LOG_TAG, "using device address: %02x %02x %02x %02x %02x %02x", rnd_addr[0], rnd_addr[1], rnd_addr[2], rnd_addr[3], rnd_addr[4], rnd_addr[5]);

  esp_err_t status;
  if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
    ESP_LOGE(LOG_TAG, "gap register error: %s", esp_err_to_name(status));
    //return;
  }

  if ((status = esp_ble_gap_set_rand_addr(rnd_addr)) != ESP_OK) {
    ESP_LOGE(LOG_TAG, "couldn't set random address: %s", esp_err_to_name(status));
    return;
  }
  
  advertiseMkData(commandDataHello, sizeof(commandDataHello));

  ESP_LOGI(LOG_TAG, "application initialized");
}

size_t parseMotorData(String data, uint8_t* dataArray) {
  size_t count = 0;
  char* token = strtok((char*)data.c_str(), ",");
  
  while (token != NULL && count < 4) {
    dataArray[count] = atoi(token);
    token = strtok(NULL, ",");
    count++;
  }
  
  return count;
}

void loop() {
  if(Serial.available()){
    uint8_t c = Serial.read();
    switch(c){
      case 'm':{
        String incomingData = Serial.readStringUntil('\n');
        uint8_t motorData[4];
        size_t motorDataCount = parseMotorData(incomingData, motorData);
        if (motorDataCount == 4) {
          setMotors(motorData[0],motorData[1],motorData[2],motorData[3]);
          Serial.println(incomingData);
        }
      }break;
      case 'i':{
        Serial.println("init");
        advertiseMkData(commandDataHello, sizeof(commandDataHello));
      }break;
      case ' ':{
        Serial.println("stop");
        setMotors(128, 128, 0, 128);
      }break;
      case 'w':{
        Serial.println("fwd");
        setMotors(128, 255, 0, 255);
      }break;
      case 'a':{
        Serial.println("left");
        setMotors(1, 128, 0, 255);
      }break;
      case 's':{
        Serial.println("back");
        setMotors(128, 1, 0, 255);
      }break;
      case 'd':{
        Serial.println("right");
        setMotors(255, 128, 0, 1);
      }break;
      case 'q':{
        Serial.println("only a ccw");
        setMotors(1, 1, 0, 128);
      }break;
      case 'e':{
        Serial.println("only a cw");
        setMotors(1, 255, 0, 128);
      }break;
    }
  }
}
