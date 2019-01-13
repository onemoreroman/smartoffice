#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

// Sensors
#define DHTPIN            7         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

// Server
char serverAddress[] = "ec2-3-121-222-156.eu-central-1.compute.amazonaws.com";  // server address
int port = 80;
String auth_user = "test_user_01";
String auth_password = "test_user_01";
int statusCode = 0;
int samples[3];
String response;

// Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC address
IPAddress ip(192, 168, 0, 177);  // if DHCP fails
IPAddress myDns(192, 168, 0, 1);  // if DHCP fails 
EthernetClient eth_client;
HttpClient http_client = HttpClient(eth_client, serverAddress, port);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize sensors
  dht.begin();
  analogReference(EXTERNAL);
 
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop() {
  // Ask sensors
  delay(10000);
  float t1 = dht.readTemperature();
  uint8_t i;
  float r2, t2;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< 3; i++) {
   samples[i] = analogRead(A5);
   delay(10);
  }
 
  // average all the samples out
  r2 = 0;
  for (i=0; i< 3; i++) {
     r2 += samples[i];
  }
  r2 /= 3;
 
  // convert the value to resistance
  r2 = 1023 / r2 - 1;
  r2 = 10000 / r2;
  t2 = r2 / 10000;     // (R/Ro)
  t2 = log(t2);                  // ln(R/Ro)
  t2 /= 3500;                   // 1/B * ln(R/Ro)
  t2 += 1.0 / (25 + 273.15); // + (1/To)
  t2 = 1.0 / t2;                 // Invert
  t2 -= 273.15;                         // convert to C
 
  // Generate data
  String postData1 = "{\"sensor_name\":\"dht11_temp\","; 
  postData1 += "\"sensor_value\":";
  postData1 += t1;
  postData1 += "}";
  Serial.println(postData1);

  String postData2 = "{\"sensor_name\":\"ntc10k_temp\","; 
  postData2 += "\"sensor_value\":";
  postData2 += t2;
  postData2 += "}";
  Serial.println(postData2);

  // Send requests
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postData1.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postData1);  
  http_client.endRequest();
  
  // Read response
  response = http_client.responseBody();
  statusCode = http_client.responseStatusCode();
  Serial.println(statusCode);
  Serial.println(response);

  
  
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postData2.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postData2);  
  http_client.endRequest();
  
  // Read response
  response = http_client.responseBody();
  statusCode = http_client.responseStatusCode();
  Serial.println(statusCode);
  Serial.println(response);
  
}
