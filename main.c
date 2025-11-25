#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define GREEN_LED 2
#define RED_LED 15
#define GAS_SENSOR_PIN 34

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* topic_pub = "wokwi/iot/airquality/data";

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const int AIR_QUALITY_GOOD = 600;
const int AIR_QUALITY_BAD = 1000;

unsigned long lastReconnectAttempt = 0;

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
  if (millis() - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = millis();
    
    Serial.println("Tentando conectar ao broker MQTT...");

    String clientId = "ESP32-AirQuality-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT conectado!");
    } else {
      Serial.print("Falha MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" - Tentando novamente em 5s...");
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Sistema iniciado!");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
  }

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  int gasRaw = analogRead(GAS_SENSOR_PIN);
  float gasPPM = map(gasRaw, 0, 4095, 0, 2000);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Erro ao ler DHT!");
    delay(2000);
    return;
  }

  bool tempBad = (temperature < 18.0 || temperature > 26.0);
  bool humBad  = (humidity < 30.0 || humidity > 60.0);
  bool gasBad  = (gasPPM > AIR_QUALITY_BAD);

  if (tempBad || humBad || gasBad) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }

  String payload = "{";
  payload += "\"temperatura\":" + String(temperature, 1) + ",";
  payload += "\"humidade\":" + String(humidity, 1) + ",";
  payload += "\"gas_ppm\":" + String(gasPPM, 0);
  payload += "}";

  Serial.println(payload);

  if (client.connected()) {
    client.publish(topic_pub, payload.c_str());
  }

  delay(2000);
}