#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

// Sensors
#define DHTPIN            7         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

// Server
char serverAddress[] = "lykova11.ru";  // server address
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
  float t1, t2;
  String postdata1, postdata2;
  float N = 5;
  uint8_t i;
  
  // Ask sensor 1
  t1 = dht.readTemperature();
  for (i=2; i<=N; i++) {
    t1 = (t1*(i-1)+dht.readTemperature())/i;
    delay(100);
  }

  // Ask sensor 2
  float r2;
  r2 = analogRead(A5);
  for (i=2; i<=N; i++) {
    r2 = (r2*(i-1)+analogRead(A5))/i;
    delay(100);
  }
 
  r2 = 1023 / r2 - 1;
  r2 = 10000 / r2;
  t2 = r2 / 10000;     // (R/Ro)
  t2 = log(t2);                  // ln(R/Ro)
  t2 /= 3500;                   // 1/B * ln(R/Ro)
  t2 += 1.0 / (25 + 273.15); // + (1/To)
  t2 = 1.0 / t2;                 // Invert
  t2 -= 273.15;                         // convert to C
 
  // Generate data
  postdata1 = "{\"sensor_name\":\"dht11_temp\","; 
  postdata1 += "\"sensor_value\":";
  postdata1 += t1;
  postdata1 += "}";
  //Serial.println(postdata1);

  postdata2 = "{\"sensor_name\":\"ntc10k_temp\","; 
  postdata2 += "\"sensor_value\":";
  postdata2 += t2;
  postdata2 += "}";
  //Serial.println(postdata2);

  // Send requests
  Serial.println(postdata1);
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postdata1.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postdata1);  
  http_client.endRequest();
  response = http_client.responseBody();
  Serial.println(response);

  Serial.println(postdata2);
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postdata2.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postdata2);  
  http_client.endRequest();
  response = http_client.responseBody();
  Serial.println(response);

  // wait
  Serial.println("  ");
  delay(60000);
}
