#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include "mqtt_client.h"

// Cliente MQTT global accesible desde otros módulos
extern esp_mqtt_client_handle_t client;

void wifi_init(void);
void mqtt_init(void);

#endif
