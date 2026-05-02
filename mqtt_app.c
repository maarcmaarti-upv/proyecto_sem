#include "mqtt_app.h"
#include "esp_log.h"
#include "mqtt_client.h"

#define MQTT_URI  "mqtt://mqtt.dsic.upv.es"
#define MQTT_USER "giirob"
#define MQTT_PASS "UPV2024"

static esp_mqtt_client_handle_t client = NULL;

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_URI,
        .credentials.username = MQTT_USER,
        .credentials.authentication.password = MQTT_PASS,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    printf("MQTT iniciado\n");
}

void mqtt_publish(const char *topic, const char *msg)
{
    if (client == NULL) return;

    esp_mqtt_client_publish(client, topic, msg, 0, 1, 0);
}
