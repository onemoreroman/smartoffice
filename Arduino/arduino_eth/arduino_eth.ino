#include <Adafruit_CCS811.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

// Sensors
Adafruit_CCS811 ccs;
DHT dht(7, DHT11);

// Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC address
EthernetClient eth_client;

// Server
String auth_user = "test_user_01";
String auth_password = "test_user_01";
HttpClient http_client = HttpClient(eth_client, "lykova11.ru", 80);

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
  http_client.endRequest();
  Serial.println(http_client.responseBody());
}

void setup() {
  // init serial
  Serial.begin(9600);
  while(!Serial);

  // init sensors
  dht.begin();
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
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
  v = dht.readHumidity();
  postdata = prep_post_data("dht11_humi", v);
  send_http_request(http_client, postdata);
  delay(1000);

  // Ask sensor 2
  v = read_avg_vol(A0, 4);
  v = volt_to_temp(v, 8450, 3500, 25);
  postdata = prep_post_data("ntc10k_temp", v);
  send_http_request(http_client, postdata);
  delay(1000);

  // Ask sensor 3
  v = read_avg_vol(A1, 4);
  v = volt_to_temp(v, 8760, 3500, 25);
  postdata = prep_post_data("ntc10k_temp1", v);
  send_http_request(http_client, postdata);
  delay(1000);
  
  // Ask sensor 4
  v = read_avg_vol(A3, 4);
  v = volt_to_temp(v, 8530, 3500, 25);
  postdata = prep_post_data("ntc10k_temp2", v);
  send_http_request(http_client, postdata);
  delay(1000);
  
  // Ask sensor 5
  if(ccs.available()){
    if(!ccs.readData()){
      v = ccs.geteCO2();
      postdata = prep_post_data("ccs811_eco2", v);
      delay(1000);
      send_http_request(http_client, postdata);
      delay(1000);
      v = ccs.getTVOC();
      postdata = prep_post_data("ccs811_tvoc", v);
      delay(1000);
      send_http_request(http_client, postdata);
    }
  }  
  delay(60000);
}
