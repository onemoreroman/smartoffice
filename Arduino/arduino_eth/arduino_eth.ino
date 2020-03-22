#include <sSense-CCS811.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

// Sensors
CCS811 ssenseCCS811;
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

String send_http_request(HttpClient http_client, String postdata) {
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postdata.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postdata);  
  http_client.endRequest();
  return http_client.responseBody();
}

void setup() {
  // open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // initialize sensors
  dht.begin();
  if(!ssenseCCS811.begin(uint8_t(I2C_CCS811_ADDRESS), uint8_t(CCS811_WAKE_PIN), driveMode_1sec))
    Serial.println("Initialization CCS811 failed.");
 
  // initialize ethernet
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);
}

void loop() {
  float v;
  String postdata, response;

  // Ask sensor 1
  v = dht.readHumidity();
  postdata = prep_post_data("dht11_humi", v);
  Serial.println(postdata);
  response = send_http_request(http_client, postdata);
  Serial.println(response);

  // Ask sensor 2
  v = read_avg_vol(A0, 4);
  v = volt_to_temp(v, 8000, 3500, 8);
  postdata = prep_post_data("ntc10k_temp1", v);
  response = send_http_request(http_client, postdata);
  Serial.println(response);

  // Ask sensor 3
  v = read_avg_vol(A3, 4);
  v = volt_to_temp(v, 10000, 3500, 27);
  postdata = prep_post_data("ntc10k_temp", v);
  response = send_http_request(http_client, postdata);
  Serial.println(response);
  
  // Ask sensor 4
  v = ssenseCCS811.getCO2();
  postdata = prep_post_data("ccs811_co2", v);
  response = send_http_request(http_client, postdata);
  Serial.println(response);
  v = ssenseCCS811.gettVOC();
  postdata = prep_post_data("ccs811_voc", v);
  response = send_http_request(http_client, postdata);
  Serial.println(response);
  
  delay(2000);
}
