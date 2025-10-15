#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define TXD 17
#define RXD 16

// Wi-Fi
const char* ssid = "Educere ";
const char* password = "Educere2023";

// MQTT Broker
const char* mqtt_server = "192.168.2.25";
const char* topico = "sensor/temperatura";
const int mqtt_port = 1883;

char jsonMsg[200];
int index1, index2, index3, index4;
float temperatura, umidade, peso, CO2;

WiFiClient espClient;            //WifiClient é uma classe da lib WiFi que gerencia a conexão TCP/IP no wifi //esp client é um objeto para estabelecer conexões TCP com servidores
PubSubClient client(espClient);  //Cria um objeto client da classe PubSubClient, serve para implementar clientes MQTT em esp ou arduino
//Recebe o objeto do wificlient para gerenciar a conexão tcp, ele publica e assina tópicos mqtt

void reconnect() {
  while (!client.connected()) {  //Cria ClientId diferente
    Serial.print("Conectando ao broker MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);  //cria o id aleatório da esp

    if (client.connect("ESP32A") {  //Tenta estabelecer a conexão com BROKER
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state()); //Tipo de erro 0-5
      Serial.println(" tentando novamente em 5s...");
      delay(5000);      
    }
  }
}
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD, TXD);

  // Conecta ao Wi-Fi          LUCAS BOSTÃO, DEMOLERI FEMININO FOREVER SARA FIA DE JOAQUIM E ROBERTO E DRENIR EB 
  WiFi.disconnect(true, true);
  delay(500);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado ao WiFi! IP: ");
  Serial.println(WiFi.localIP());

  // Configura broker MQTT
  client.setServer(mqtt_server, mqtt_port);
}
void loop() {
  // Verifica se o client está conectado, reconnect se necessário
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  //Processar mensagens recebidas, manter a conexão ativa e executar callbacks de mensagem. Sem, a conexão pode cair

  String dados;

  if (Serial1.available()) {
    dados = Serial1.readStringUntil('\n');  // lê até encontrar '\n'
    dados.trim();  // remove espaços, \r e \n extras
    Serial.print("Recebido do ESP32: ");
    Serial.println(dados);
    
    index1 = dados.indexOf(',');
    index2 = dados.indexOf(',', index1 + 1);
    index3 = dados.indexOf(',', index2 + 1);
    index4 = dados.indexOf(',', index3 + 1);

    temperatura = dados.substring(0, index1).toFloat();
    umidade = dados.substring(index1 + 1, index2).toFloat();
    peso = dados.substring(index2 + 1, index3).toFloat();
    CO2 = dados.substring(index3 + 1, index4).toFloat();

    StaticJsonDocument<200> doc;
    if (temperatura != 0.0) doc["temperatura"] = temperatura;
    if (umidade != 0.0) doc["umidade"] = umidade;
    if (peso != 0.0) doc["peso"] = peso;
    if (CO2 != 0.0) doc["CO2"] = CO2;

  if (doc.size() > 0) {
    serializeJson(doc, jsonMsg);
    Serial.print("JSON criado: ");
    Serial.println(jsonMsg);
  // Publicar mensagem no tópico
    if (client.publish(topico, jsonMsg)) { //Tenta publicar no topico
    Serial.print("Publicado!");
    } else {
    Serial.println("Falha ao publicar!"); //Caso não envie
    }
    }
  }
  delay(5000); 
}
