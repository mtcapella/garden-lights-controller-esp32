#include <WiFi.h>
// Esta libreria proporcional las funcionalidades de conectar y adminsistrar una conexion wifi
#include <AsyncTCP.h>
// esta libreria implementa una capa de transporte asincrona basada en TCP
#include <ESPAsyncWebSrv.h>
// Permite la creacion de un servidor web asincrono

const char* ssid = "MiWifi"; 
const char* password = "MiContraseñaWifi2024";

const int relayPin = 32; // Pin del modolo 1 del relé
const int buttonPin = 33; // Pin del puldador manual.
bool relayState = false; // estado del relé. Este puede ser true (encendido) o false (apagado)

IPAddress localIP(192, 168, 1, 148);    // Dirección IP estática
IPAddress gateway(192, 168, 1, 1);      // Puerta de enlace
IPAddress subnet(255, 255, 255, 0);     // Máscara de subred

AsyncWebServer server(80); // Creamos la instalacia del servidor web para escuchar las peticiones HTTP

void setup() {
  Serial.begin(115200);

  // Conexión a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi...");
  }
  Serial.println("Conexión exitosa");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configuración del pin GPIO32 como salida
  pinMode(relayPin, OUTPUT);

  // Configuración del pin del botón como entrada con resistencia pull-up interna asi no hace falta poner una resistencia externa.
  pinMode(buttonPin, INPUT_PULLUP);

  // Configuración de los endpoints
  server.on("/rele", HTTP_GET, [](AsyncWebServerRequest *request) {
    String state = relayState ? "activo" : "inactivo";
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", state);
    // Configuración del endpoint /rele para permitir CORS y que el navegador no bloquee las peticiones
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/rele/on", HTTP_POST, [](AsyncWebServerRequest *request) {
    relayState = true;
    digitalWrite(relayPin, HIGH);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Relé activado");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/rele/off", HTTP_POST, [](AsyncWebServerRequest *request) {
    relayState = false;
    digitalWrite(relayPin, LOW);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Relé desactivado");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  // Inicio del servidor
  server.begin();
}

void loop() {
  static bool buttonStatePrev = HIGH;
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && buttonStatePrev == HIGH) {
    // Cambiarña el estado del rele cuando se presione el pulsador
    relayState = !relayState;
    digitalWrite(relayPin, relayState ? HIGH : LOW);
    delay(200);  // Agregar un leve retraso para evitar rebotes en el pulsador
  }

  buttonStatePrev = buttonState;
}
