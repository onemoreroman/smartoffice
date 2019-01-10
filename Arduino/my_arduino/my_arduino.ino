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
  delay(2000);
  float t = dht.readTemperature();
//float h = dht.readHumidity();

  // Generate data
  String postData = "{\"sensor_name\":\"dht11_temp\","; 
  postData += "\"sensor_value\":";
  postData += t;
  postData += "}";
  Serial.println(postData);
  
  // Send request
  http_client.beginRequest();
  http_client.post("/office/upload_data");
  http_client.sendHeader("Content-Type", "application/json");
  http_client.sendHeader("Content-Length", postData.length());
  http_client.sendBasicAuth(auth_user, auth_password);
  http_client.beginBody();
  http_client.print(postData);  
  http_client.endRequest();
  
  // Read response
  response = http_client.responseBody();
  statusCode = http_client.responseStatusCode();
  Serial.println(statusCode);
  Serial.println(response);
}
