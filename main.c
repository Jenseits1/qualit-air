#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define GREEN_LED 2
#define RED_LED 15

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* topic_pub = "iot/airquality/data";

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

void setup_wifi() {
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");

    String clientId = "ESP32-AirQuality-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT conectado!");
    } else {
      Serial.print("Falha MQTT, rc=");
      Serial.println(client.state());
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Sistema iniciado!");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Erro ao ler DHT!");
    delay(2000);
    return;
  }

  bool tempBad = (temperature < 18.0 || temperature > 26.0);
  bool humBad  = (humidity < 30.0 || humidity > 60.0);

  if (tempBad || humBad) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }

  String payload = "{";
  payload += "\"temperatura\":" + String(temperature, 1) + ",";
  payload += "\"humidade\":" + String(humidity, 1);
  payload += "}";

  unsigned long startPublish = micros();
  bool sent = client.publish(topic_pub, payload.c_str());
  unsigned long publishTime = micros() - startPublish;

  if (sent) {
    Serial.println("MQTT enviado: " + payload);
  } else {
    Serial.println("Erro ao enviar MQTT!");
  }

  Serial.print("Tempo de envio MQTT (microssegundos): ");
  Serial.println(publishTime);

  delay(2000);
}
