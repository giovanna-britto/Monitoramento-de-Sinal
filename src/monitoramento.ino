#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Configurações Wi-Fi
const char* ssid = "Gigi";
const char* password = "12345678";

// Configurações do Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883 // Porta MQTT padrão
//#define AIO_USERNAME    "username"
//#define AIO_KEY         "senha"

// Conexão Wi-Fi e MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Feed do Adafruit IO
Adafruit_MQTT_Publish signalStrengthFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/signal_strength");

void setup() {
  Serial.begin(115200);

  // Conexão Wi-Fi
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi");

  // Conexão MQTT
  connectToMQTT();
}

void loop() {
  // Reestabelece conexão MQTT, se necessário
  if (!mqtt.connected()) {
    connectToMQTT();
  }
  mqtt.processPackets(10000);
  mqtt.ping();

  // Mede a intensidade do sinal Wi-Fi
  int signalStrength = WiFi.RSSI();
  Serial.print("Intensidade do sinal (dBm): ");
  Serial.println(signalStrength);

  // Publica os dados no Adafruit IO
  if (signalStrengthFeed.publish((float)signalStrength)) { // Converte para float para evitar ambiguidade
    Serial.println("Dados publicados com sucesso!");
  } else {
    Serial.println("Falha ao publicar os dados.");
  }

  delay(5000); // Aguarda 5 segundos antes de medir novamente
}

void connectToMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Conectando ao Adafruit IO MQTT...");
    int8_t ret = mqtt.connect(); // Obter o código de erro
    if (ret == 0) {
      Serial.println("Conectado ao Adafruit IO!");
    } else {
      Serial.print("Falha na conexão. Erro: ");
      Serial.println(mqtt.connectErrorString(ret)); // Passa o código de erro
      Serial.println("Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}
