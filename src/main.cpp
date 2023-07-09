#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const int motorPin = D5;       // Pin GPIO utilizado para controlar el motor
const int ledPin = D7;         // Pin GPIO utilizado para controlar el LED
const int numPixels = 1;       // Número total de LEDs

Adafruit_NeoPixel pixels(numPixels, ledPin, NEO_GRB + NEO_KHZ800);

ESP8266WebServer server(80);

unsigned long tiempoEncendido = 120000;  // 2 minutos en milisegundos
unsigned long tiempoApagado = 300000;    // 5 minutos en milisegundos
unsigned long tiempoInicio = 0;
bool motorEncendido = false;

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Control del motor y LED</h1>";
  html += "<p>Estado del motor: ";
  
  if (motorEncendido) {
    html += "Encendido</p>";
  } else {
    html += "Apagado</p>";
  }
  
  html += "<p><a href=\"/encenderMotor\">Encender Motor</a> | <a href=\"/apagarMotor\">Apagar Motor</a></p>";
  html += "<p>Estado del LED: ";
  
  if (pixels.getPixelColor(0) != 0) {
    html += "Encendido</p>";
  } else {
    html += "Apagado</p>";
  }
  
  html += "<p><a href=\"/encenderLED\">Encender LED</a> | <a href=\"/apagarLED\">Apagar LED</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleEncenderMotor() {
  digitalWrite(motorPin, HIGH);
  server.send(200, "text/plain", "Motor encendido");
  motorEncendido = true;
  tiempoInicio = millis(); // Reinicia el tiempo de inicio
}

void handleApagarMotor() {
  digitalWrite(motorPin, LOW);
  server.send(200, "text/plain", "Motor apagado");
  motorEncendido = false;
}

void handleEncenderLED() {
  pixels.setPixelColor(0, 255, 255, 255);  // Enciende el LED en blanco
  pixels.show();
  server.send(200, "text/plain", "LED encendido");
}

void handleApagarLED() {
  pixels.setPixelColor(0, 0, 0, 0);    // Apaga el LED
  pixels.show();
  server.send(200, "text/plain", "LED apagado");
}

void setup() {
  pinMode(motorPin, OUTPUT);
  
  pixels.begin();
  pixels.setPixelColor(0, 0, 0, 0);    // Inicializa el LED apagado
  pixels.show();
  
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.on("/", handleRoot);
  server.on("/encenderMotor", handleEncenderMotor);
  server.on("/apagarMotor", handleApagarMotor);
  server.on("/encenderLED", handleEncenderLED);
  server.on("/apagarLED", handleApagarLED);

  server.begin();

  Serial.println("Web Server Initiated");
}

void loop() {
  server.handleClient();
  
  if (motorEncendido) {
    unsigned long tiempoActual = millis() - tiempoInicio;
    
    if (tiempoActual >= tiempoEncendido) {
      digitalWrite(motorPin, LOW); // Apaga el motor después de tiempoEncendido
    }
    if (tiempoActual >= (tiempoEncendido + tiempoApagado)) {
      digitalWrite(motorPin, HIGH); // Enciende el motor después de tiempoEncendido + tiempoApagado
      tiempoInicio = millis(); // Reinicia el tiempo de inicio
    }
  }
}
