#include <Adafruit_CCS811.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

// Sensors
Adafruit_CCS811 ccs;
DHT dht(7, DHT11);

// Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC address
EthernetClient eth;

// Server
HttpClient http = HttpClient(eth, SECRET_SERVER_HOST, SECRET_SERVER_PORT);

float readAvgVoltage(int analog_pin, int N) {
  float v; int i;
  v = analogRead(analog_pin);
  for (i=2; i<=N; i++) {
    v = (v*(i-1)+analogRead(analog_pin))/i;
    delay(100);
  }
  return 5*(v/1023);
}

float convertVoltageToTemp(float v, float R0, float B, float T0) {
  float T, RtoR0;
  RtoR0 = v/(5-v);
  T = log(RtoR0)/B;
  T += 1.0/(T0+273.15);
  T = 1.0/T;
  T -= 273.15;
  return T;
}

void postJSON(String& json) {
  http.beginRequest();
  http.post("/office/upload_data");
  http.sendHeader("Content-Type", "application/json");
  http.sendHeader("Content-Length", json.length());
  http.sendBasicAuth(SECRET_SERVER_USER, SECRET_SERVER_PASSWORD);
  http.beginBody();
  http.print(json);
  http.print("\r\n");
  delay(200); // avoid "Broken pipe [core/writer.c line 306] during POST /office/upload_data"
//  Serial.println(http.readString()); // Print response for debugging
  http.endRequest();
  http.stop();
  json = "";
}

void jsonifySensorData(String sensorName, float sensorValue, String& json)
{
  StaticJsonDocument<100> doc;

  doc["sensor_name"] = sensorName;
  doc["sensor_value"] = sensorValue;

  serializeJson(doc, json);
}

void setup() {
  // init serial
  Serial.begin(9600);
  while(!Serial);

  // init sensors
  dht.begin();
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
  }
 
  // init ethernet
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);
}

void loop() {
  float v;
  String json;

//  // Sensor 1: DHT11, turned out to be unstable :(
//  v = dht.readHumidity();
//  jsonifySensorData("dht11_humi", v, json);
//  postJSON(json);
//  delay(2000);

  // Sensor 2: NTC10k 
  v = readAvgVoltage(A0, 5);
  v = convertVoltageToTemp(v, 8450, 3500, 25);
  jsonifySensorData("ntc10k_temp2", v, json);
  postJSON(json);
  delay(2000);

  // Sensor 3: NTC10k
  v = readAvgVoltage(A1, 5);
  v = convertVoltageToTemp(v, 8760, 3500, 25);
  jsonifySensorData("ntc10k_temp", v, json);
  postJSON(json);
  delay(2000);
  
  // Sensor 4: NTC10k
  v = readAvgVoltage(A3, 5);
  v = convertVoltageToTemp(v, 8530, 3500, 25);
  jsonifySensorData("ntc10k_temp1", v, json);
  postJSON(json);
  delay(2000);
  
  // Sensor 5: CCS811
  if(ccs.available()){
    if(!ccs.readData()){
      v = ccs.getTVOC();
      jsonifySensorData("ccs811_tvoc", v, json);
      postJSON(json);
    }
  }  
  delay(10000);
}
