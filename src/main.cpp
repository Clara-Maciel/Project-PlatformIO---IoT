#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <DHT.h>

// === CONFIGURAÇÕES DO DHT ===
#define DHTPIN D4       
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// === CONFIGURAÇÃO DO AP ===
const char* ssid = "oiDaSilva";
const char* password = "senhadomuseu";

ESP8266WebServer server(80);

// === Página inicial (lê arquivo HTML do SPIFFS) ===
void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "Erro ao abrir index.html");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

// === Rota JSON ===
void handleDados() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    server.send(200, "application/json", "{\"temperatura\":null,\"umidade\":null}");
    return;
  }

  String json = "{";
  json += "\"temperatura\":" + String(t, 1) + ",";
  json += "\"umidade\":" + String(h, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  if (!SPIFFS.begin()) {
    Serial.println("Erro ao montar SPIFFS!");
    return;
  }

  // Criar Access Point
  WiFi.softAP(ssid, password);
  Serial.println("AP criado!");
  Serial.print("Acesse o Wi-Fi: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/dados", handleDados);
  server.begin();

  Serial.println("Servidor iniciado!");
}

void loop() {
  server.handleClient();
  delay(2000); 
}