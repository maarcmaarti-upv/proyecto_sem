#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include "mqtt_client.h"

// Inicializa WiFi
void wifi_init(void);

// Inicializa MQTT y devuelve el cliente
esp_mqtt_client_handle_t mqtt_init(void);

#endif
