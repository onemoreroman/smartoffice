#include <WiFiEsp.h>
#include <sSense-CCS811.h>
#include <WiFiEsp.h>
#include "SoftwareSerial.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

// Wi-Fi
char ssid[] = "ELTX-2.4GHz_WiFi"; // your network SSID (name)
char pass[] = "GP2F050097"; // your network password
int status = WL_IDLE_STATUS;
WiFiEspClient client;

// Remote server
char serverAddress[] = "lykova11.ru";
int port = 80;
String auth_user = "test_user_01";
String auth_password = "test_user_01";
int statusCode = 0;
int samples[3];
String response;

// Sensors
CCS811 ssenseCCS811;

void setup()
{
  // open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  delay(3000);

  // initialize CCS811
  if(!ssenseCCS811.begin(uint8_t(I2C_CCS811_ADDRESS), uint8_t(CCS811_WAKE_PIN), driveMode_1sec))
    Serial.println("Initialization CCS811 failed.");

  pinMode(LED_BUILTIN, OUTPUT); // initialize digital pin LED_BUILTIN as an output.
  WiFi.init(&Serial1); // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

float read_avg_vol(int analog_pin, int N) {
  float v; int i;
  v = analogRead(analog_pin);
  for (i=2; i<=N; i++) {
    v = (v*(i-1)+analogRead(analog_pin))/i;
    delay(100);
  }
  return 5*(v/1023);
}

float vol2temp(float v, float R0, float B) {
  float T, RtoR0;
  RtoR0 = v/(5-v);
  T = log(RtoR0)/B; // ln(R/Ro)/B
  T += 1.0/(25+273.15); // + (1/To)
  T = 1.0/T; // invert
  T -= 273.15; // convert to C
  return T;
}

void loop() {
  float sensor1_t, sensor2_co2, sensor2_voc, v;
  String postdata;
  
  // Ask sensor ntc10k
  v = read_avg_vol(A0, 4);
  sensor1_t = vol2temp(v, 8960, 3500);
  
  // Ask sensor CSS  
  if (ssenseCCS811.checkDataAndUpdate())
  {
    sensor2_co2 = ssenseCCS811.getCO2();
    sensor2_voc = ssenseCCS811.gettVOC();
  }
  else if (ssenseCCS811.checkForError())
  {
    ssenseCCS811.printError();
  }

  // Generate data
  postdata = "{\"sensor_name\":\"ntc10k_1\","; 
  postdata += "\"sensor_value\":";
  postdata += sensor1_t;
  postdata += "}";
  
  // Send request
  if (client.connect(serverAddress, 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.cc");
    client.println("Connection: close");
    client.println();
  }
  // wait
  delay(2000);
}
