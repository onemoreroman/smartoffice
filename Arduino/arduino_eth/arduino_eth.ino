#include <Adafruit_CCS811.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>
#include "smartoffice_secrets.h"

// Sensors
Adafruit_CCS811 ccs;
DHT dht(7, DHT11);

// Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC address
EthernetClient eth_client;

// Server
String auth_user = SECRET_SMARTOFFICE_USER;
String auth_password = SECRET_SMARTOFFICE_PASSWORD;
HttpClient http_client = HttpClient(eth_client, SECRET_SMARTOFFICE_HOST, SECRET_SMARTOFFICE_PORT);

float read_avg_vol(int analog_pin, int N) {
  float v; int i;
  v = analogRead(analog_pin);
  for (i=2; i<=N; i++) {
    v = (v*(i-1)+analogRead(analog_pin))/i;
    delay(100);
  }
  return 5*(v/1023);
}

float volt_to_temp(float v, float R0, float B, float T0) {
  float T, RtoR0;
  RtoR0 = v/(5-v);
  T = log(RtoR0)/B;
  T += 1.0/(T0+273.15);
  T = 1.0/T;
  T -= 273.15;
  return T;
}

String prep_post_data(String sensor, float value) {
  String s;
  s = "{\"name\": \"";
  s += sensor;
  s += "\"";
  s += ", \"value\": ";
  s += value;
  s += "}";
  return s;
}

void send_http_request(HttpClient http_client, String postdata) {
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postdata.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postdata);  
  delay(300); // avoid "Broken pipe [core/writer.c line 306] during POST /office/upload_data"
  http_client.endRequest();
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
  String postdata;

  // Ask sensor 1
  // Serial.println("s1");
  v = dht.readHumidity();
  postdata = prep_post_data("dht11_humi", v);
  // Serial.println(postdata);
  send_http_request(http_client, postdata);
  delay(1000);

  // Ask sensor 2
  // Serial.println("s2");
  v = read_avg_vol(A0, 5);
  v = volt_to_temp(v, 8450, 3500, 25);
  postdata = prep_post_data("ntc10k_temp2", v);
  // Serial.println(postdata);
  send_http_request(http_client, postdata);
  delay(1000);

  // Ask sensor 3
  // Serial.println("s3");
  v = read_avg_vol(A1, 5);
  v = volt_to_temp(v, 8760, 3500, 25);
  postdata = prep_post_data("ntc10k_temp", v);
  // Serial.println(postdata);
  send_http_request(http_client, postdata);
  delay(1000);
  
  // Ask sensor 4
  // Serial.println("s4");
  v = read_avg_vol(A3, 5);
  v = volt_to_temp(v, 8530, 3500, 25);
  postdata = prep_post_data("ntc10k_temp1", v);
  // Serial.println(postdata);
  send_http_request(http_client, postdata);
  delay(1000);
  
  // Ask sensor 5
  // Serial.println("s5");
  if(ccs.available()){
    if(!ccs.readData()){
      v = ccs.getTVOC();
      postdata = prep_post_data("ccs811_tvoc", v);
      // Serial.println(postdata);
      send_http_request(http_client, postdata);
    }
  }  
  delay(10000);
}
