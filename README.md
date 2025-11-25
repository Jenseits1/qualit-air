# QualitAir

Sistema de monitoramento de temperatura, umidade e qualidade do ar usando ESP32, sensor DHT22, sensor de gás MQ-135 e comunicação MQTT.

## Descrição

Este projeto monitora continuamente a temperatura, umidade e qualidade do ar do ambiente, publicando os dados via MQTT e sinalizando visualmente quando os parâmetros estão fora da faixa ideal através de LEDs. O sensor de gás é simulado através de um potenciômetro deslizante para validação da lógica do sistema.

## Componentes Necessários

-   **ESP32** (placa de desenvolvimento)
-   **Sensor DHT22** (temperatura e umidade)
-   **Potenciômetro Deslizante** (simulação do sensor MQ-135)
-   **LED Verde** (indicador de condições normais)
-   **LED Vermelho** (indicador de alerta)
-   **Resistores** 220Ω para os LEDs
-   Cabos jumper para conexões

## Diagrama de Conexões

### Simulação no Wokwi

🔗 **[Visualizar projeto completo no Wokwi](https://wokwi.com/projects/448265554947206145)**

Você pode testar e simular o projeto diretamente no navegador através do link acima!

## Bibliotecas Necessárias

### Para Arduino IDE

Instale as seguintes bibliotecas através do Arduino IDE (Sketch → Include Library → Manage Libraries):

-   **WiFi** (integrada ao ESP32)
-   **PubSubClient** v2.8 por Nick O'Leary
-   **DHT sensor library** por Adafruit

### Para Wokwi

O projeto já está configurado com o arquivo `libraries.txt`:

```txt
DHT sensor library
PubSubClient@2.8
WiFi
```

Basta abrir o projeto no Wokwi que as bibliotecas serão carregadas automaticamente!

## Configuração

### 1. Credenciais WiFi

Edite as seguintes linhas no código para sua rede:

```cpp
const char* ssid = "Wokwi-GUEST";
const char* password = "";
```

### 2. Broker MQTT

O projeto usa o broker público `iot.coreflux.cloud`:

```cpp
const char* mqtt_server = "iot.coreflux.cloud";
const int mqtt_port = 1883;
```

### 3. Tópico MQTT

Os dados são publicados no tópico:

```
wokwi/iot/airquality/data
```

## Faixas de Operação

### Condições Normais (LED Verde)

-   **Temperatura:** 18°C a 26°C
-   **Umidade:** 30% a 60%
-   **Qualidade do ar:** até 1000 PPM

### Condições de Alerta (LED Vermelho)

-   Temperatura < 18°C ou > 26°C
-   Umidade < 30% ou > 60%
-   Qualidade do ar > 1000 PPM

## Formato dos Dados MQTT

Os dados são publicados em formato JSON:

```json
{
	"temperatura": 23.5,
	"humidade": 45.2,
	"gas_ppm": 850
}
```

## Como Usar

1. Instale as bibliotecas necessárias no Arduino IDE
2. Configure as credenciais WiFi no código
3. Conecte os componentes conforme o diagrama de pinos
4. Faça upload do código para o ESP32
5. Abra o Serial Monitor (115200 baud) para acompanhar os logs
6. Ajuste o potenciômetro deslizante para simular diferentes níveis de gás
7. Os dados serão enviados a cada 2 segundos
