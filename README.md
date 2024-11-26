# **Documentação: Monitoramento de Sinal Wi-Fi com ESP32 e Adafruit IO**

### **1. Objetivo**
Desenvolver uma solução IoT utilizando ESP32 para medir a intensidade do sinal Wi-Fi em dBm e publicar os dados em tempo real na plataforma **Adafruit IO**. A solução inclui a criação de uma dashboard no Adafruit IO para visualização gráfica contínua dos valores coletados.

---

### **2. Hardware Utilizado**
- **ESP32**: Microcontrolador Wi-Fi.
- **Fonte USB**: Para alimentar o ESP32.
- **Computador com Arduino IDE**: Para desenvolvimento e upload do código.
- **Conexão Wi-Fi**: Rede local para comunicação com a internet.

---

### **3. Software Utilizado**
- **Arduino IDE**:
  - Configurado com o suporte à placa ESP32.
- **Bibliotecas**:
  - `WiFi.h` (nativa do ESP32 para conexões Wi-Fi).
  - `Adafruit MQTT Library` (para comunicação MQTT com o Adafruit IO).

- **Adafruit IO**:
  - Plataforma online para visualização e manipulação de dados IoT.

---

### **4. Etapas do Projeto**

#### **4.1. Configuração do Adafruit IO**
1. **Conta Adafruit IO**:
   - Acesse [Adafruit IO](https://io.adafruit.com/) e crie uma conta.
2. **Feed**:
   - Crie um feed chamado `signal_strength`.
3. **Dashboard**:
   - Adicione um gráfico de linha ao dashboard e vincule-o ao feed `signal_strength`.

---

#### **4.2. Desenvolvimento do Código**
1. **Configurações iniciais**:
   - Configure o nome e a senha da rede Wi-Fi.
   - Adicione as credenciais do Adafruit IO (`AIO_USERNAME` e `AIO_KEY`).

2. **Funções principais**:
   - Conexão Wi-Fi: Estabelece conexão com a rede Wi-Fi configurada.
   - Comunicação MQTT: Configura a conexão MQTT para publicar os dados no feed.
   - Leitura de intensidade do sinal: Mede o RSSI (Received Signal Strength Indicator) usando `WiFi.RSSI()`.

3. **Envio de dados**:
   - Publica os valores de RSSI no feed `signal_strength` a cada 5 segundos.

---

#### **4.3. Código Implementado**

_Acesse o código por meio do seguinte [link](./src/monitoramento.ino)_

**1. Inclusão de Bibliotecas**
```cpp
#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
```
- **WiFi.h**: Biblioteca nativa do ESP32 que gerencia conexões Wi-Fi.
- **Adafruit_MQTT.h** e **Adafruit_MQTT_Client.h**: Bibliotecas que permitem a comunicação via protocolo MQTT com o Adafruit IO.

**2. Configurações de Rede e Adafruit IO**
```cpp
const char* ssid = "Gigi";            // Nome da rede Wi-Fi
const char* password = "12345678";   // Senha da rede Wi-Fi

#define AIO_SERVER      "io.adafruit.com"  // Servidor MQTT do Adafruit IO
#define AIO_SERVERPORT  1883              // Porta MQTT padrão
#define AIO_USERNAME    "username"          // Nome de usuário do Adafruit IO
#define AIO_KEY         "senha" // Chave de API do Adafruit IO
```
- **ssid**: Nome da rede Wi-Fi à qual o ESP32 se conectará.
- **password**: Senha da rede Wi-Fi.
- **AIO_SERVER** e **AIO_SERVERPORT**: Configurações do servidor MQTT do Adafruit IO.
- **AIO_USERNAME** e **AIO_KEY**: Credenciais para autenticação no Adafruit IO.

**3. Configuração do Cliente MQTT**
```cpp
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
```
- **WiFiClient client**: Objeto que gerencia a conexão TCP/IP para o MQTT.
- **Adafruit_MQTT_Client mqtt**: Configura o cliente MQTT com o servidor, porta e credenciais.

**4. Configuração do Feed MQTT**
```cpp
Adafruit_MQTT_Publish signalStrengthFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/signal_strength");
```
- **signalStrengthFeed**: Feed MQTT vinculado ao feed `signal_strength` no Adafruit IO. Esse feed receberá os dados de intensidade do sinal Wi-Fi.

**5. Função `setup`**
```cpp
void setup() {
  Serial.begin(115200); // Inicia a comunicação serial para monitoramento

  // Conexão Wi-Fi
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password); // Conecta ao Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("."); // Indica progresso da conexão
  }
  Serial.println("\nConectado ao Wi-Fi");

  // Conexão MQTT
  connectToMQTT(); // Conecta ao Adafruit IO
}
```
- **`Serial.begin(115200)`**: Configura a comunicação serial a 115200 bps para monitorar a execução.
- **`WiFi.begin(ssid, password)`**: Inicia a conexão à rede Wi-Fi.
- **`connectToMQTT()`**: Função que estabelece a conexão MQTT com o Adafruit IO.


**6. Função `loop`**
```cpp
void loop() {
  // Reestabelece conexão MQTT, se necessário
  if (!mqtt.connected()) {
    connectToMQTT(); // Reconecta ao MQTT se desconectado
  }
  mqtt.processPackets(10000); // Processa pacotes MQTT
  mqtt.ping();                // Envia um "ping" para manter a conexão ativa

  // Mede a intensidade do sinal Wi-Fi
  int signalStrength = WiFi.RSSI();
  Serial.print("Intensidade do sinal (dBm): ");
  Serial.println(signalStrength);

  // Publica os dados no Adafruit IO
  if (signalStrengthFeed.publish((float)signalStrength)) { // Converte para float
    Serial.println("Dados publicados com sucesso!");
  } else {
    Serial.println("Falha ao publicar os dados.");
  }

  delay(5000); // Aguarda 5 segundos antes de medir novamente
}
```
- **`mqtt.connected()`**: Verifica se a conexão MQTT está ativa.
- **`mqtt.processPackets()`**: Processa pacotes recebidos via MQTT.
- **`mqtt.ping()`**: Mantém a conexão MQTT ativa.
- **`WiFi.RSSI()`**: Mede a intensidade do sinal Wi-Fi em dBm.
- **`signalStrengthFeed.publish()`**: Publica os dados de RSSI no feed do Adafruit IO.
- **`delay(5000)`**: Pausa de 5 segundos entre as medições.

---

**7. Função `connectToMQTT`**
```cpp
void connectToMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Conectando ao Adafruit IO MQTT...");
    int8_t ret = mqtt.connect(); // Obter o código de erro
    if (ret == 0) {
      Serial.println("Conectado ao Adafruit IO!");
    } else {
      Serial.print("Falha na conexão. Erro: ");
      Serial.println(mqtt.connectErrorString(ret)); // Exibe mensagem de erro
      Serial.println("Tentando novamente em 5 segundos...");
      delay(5000); // Espera antes de tentar reconectar
    }
  }
}
```
- **`mqtt.connect()`**: Estabelece a conexão MQTT com o Adafruit IO.
- **`mqtt.connectErrorString(ret)`**: Retorna uma mensagem explicando o código de erro caso a conexão falhe.


#### **Resumo das Funcionalidades**
1. O ESP32 conecta-se à rede Wi-Fi configurada.
2. Mede continuamente a intensidade do sinal Wi-Fi (RSSI).
3. Publica os valores medidos no feed `signal_strength` do Adafruit IO via MQTT.
4. Exibe logs no monitor serial, incluindo o status da conexão e o sucesso ou falha na publicação dos dados.


#### **Fluxo do Código**
1. **Configuração Inicial**:
   - O ESP32 conecta-se ao Wi-Fi e ao Adafruit IO no `setup`.
2. **Execução Contínua**:
   - No `loop`, o ESP32:
     - Garante que a conexão MQTT esteja ativa.
     - Mede a intensidade do sinal Wi-Fi.
     - Publica os dados no Adafruit IO.
3. **Reconexão Automática**:
   - Se a conexão Wi-Fi ou MQTT falhar, o ESP32 tenta reconectar automaticamente.


#### **Principais Dependências**
- **Wi-Fi estável**: Conexão à internet é essencial para comunicação com o Adafruit IO.
- **Configuração correta do Adafruit IO**: Credenciais do usuário, chave de API e feed configurados corretamente.

#### **Possíveis Melhorias**
1. **Notificações de Alerta**:
   - Adicionar notificações (ex.: e-mails ou mensagens) quando o RSSI cair abaixo de um valor crítico.
2. **Armazenamento Local**:
   - Salvar os dados coletados em um cartão SD caso a conexão com o Adafruit IO seja perdida.
3. **Multifeeds**:
   - Expandir para monitorar outras métricas, como temperatura ou umidade.


#### **Conclusão**
Este código demonstra como integrar o ESP32 com o Adafruit IO para um sistema IoT simples, eficiente e escalável. Ele pode ser usado para monitorar a qualidade do sinal Wi-Fi e servir como base para outros projetos IoT.

### **5. Testes Realizados**

_Link para acessar o vídeo: [link do vídeo](https://drive.google.com/file/d/10-t3k9w3Blj3mwo9_CHTdpsjKZBhZg23/view?usp=sharing)_

1. **Cenários Testados**:
   - Ambiente interno (sala de admissão).
   - Perto de barreiras físicas (mão).

2. **Resultados Observados**:
   - Os valores de intensidade do sinal (RSSI) diminuíram significativamente ao colocar a mão sobre o ESP.

3. **Comportamento no Dashboard**:
   - O gráfico no Adafruit IO mostrou claramente a queda do sinal e sua recuperação, demonstrando a funcionalidade do sistema.

---

### **6. Resultados**
- A intensidade do sinal Wi-Fi foi monitorada em tempo real, com dados publicados consistentemente no Adafruit IO.
- O gráfico de linha exibiu as variações de RSSI de acordo com os cenários testados.

---

### **7. Conclusão**
O projeto demonstrou com sucesso como integrar um ESP32 ao Adafruit IO para monitorar e visualizar dados em tempo real. A plataforma mostrou-se eficiente para projetos IoT simples e permite fácil expansão para novas funcionalidades.