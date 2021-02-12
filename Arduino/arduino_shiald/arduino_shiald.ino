// To upload code to Arduino+Shiald:
// 0. Make sure there is no code on Arduino without Shiald
// 1. Set shiald switches all off
// 2. Break the 0, 1 legs on shiald (cut them off)
// 3. Bend the pins of two male-male wires and put them into Arduino 0, 1 
// 4. Install shiald to Arduino making sure the wires stay in place
// 5. Use the other ends of the wires to connect:
//    Arduino RX (0) - Shiald TX (1)
//    Arduino TX (1) - Shiald RX (0)
//    i.e. we need to cross the wires
// 6. Connect Arduino+Shiald to computer and upload code
// To run uploaded code:
// 1. Disconnect USB, set P1, P2 switches to ON and reroute the wires:
//    Shiald 0 - Shiald 6
//    Shiald 1 - Shiald 7
// 2. Reconnect Arduino+Shiald to computer and open Serial port for debugging

#include <OneWire.h>
#include <WiFiEsp.h>
#include "SoftwareSerial.h"
#include "secrets.h"
#include <DallasTemperature.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// Wi-Fi
IPAddress ip;
bool x = ip.fromString(SECRET_WIFI_STATIC_IP);
SoftwareSerial Serial1(6,7);
int status = WL_IDLE_STATUS;
WiFiEspClient client;

// Sensor
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

// Server
HttpClient http = HttpClient(client, SECRET_SERVER_HOST, SECRET_SERVER_PORT);


void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  sensors.begin(); 

  WiFi.init(&Serial1);
  WiFi.config(ip);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  while (status != WL_CONNECTED) {
    status = WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
  }

}


void loop() {
  StaticJsonDocument<80> doc;
  String json;
  
  sensors.requestTemperatures();
  doc["sensor_name"] = "leha_sensor";
  doc["sensor_value"] = sensors.getTempCByIndex(0);
  serializeJsonPretty(doc, json);
  
  http.beginRequest();
  http.post("/office/upload_data");
  http.sendHeader("Content-Type", "application/json");
  http.sendHeader("Content-Length", json.length());
  http.sendBasicAuth(SECRET_SERVER_USER, SECRET_SERVER_PASSWORD);
  http.beginBody();
  http.print(json);
  http.print("\r\n");  
  delay(200); 
  http.endRequest();
  http.stop();
  
  delay(10000);
}
