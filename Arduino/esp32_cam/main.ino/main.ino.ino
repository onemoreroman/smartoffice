#include <ezButton.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define BUTTON_PIN         13
#define LED1_PIN           12
#define LONG_PRESS_TIME    1000
#define TIMEOUT_WIFI_SETUP 100000
#define BLINK_INTERVAL     300

int ledState = LOW;   // ledState used to set the LED
unsigned long previousMillis = 0;   // will store last time LED was updated
unsigned long currentMillis = 0;
unsigned long buttonPressedMillis = 0;
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
ezButton button(BUTTON_PIN);
WiFiManager wm;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  pinMode(LED1_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  button.loop(); // MUST call the loop() function first
  digitalWrite(LED1_PIN, LOW);

  if (button.isPressed())
    pressedTime = millis();

  if (button.isReleased()) {
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressDuration > LONG_PRESS_TIME) {
      wm.resetSettings();  // remove for PROD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      Serial.println("A long press is detected");
      buttonPressedMillis = millis();
      currentMillis = 0;
      previousMillis = 0;
      while (currentMillis <= TIMEOUT_WIFI_SETUP) {
        bool res;
        res = wm.autoConnect("AutoConnectAP");
        if(!res) {
          Serial.println("Failed to connect");
          // ESP.restart();
        } 
        else {
          //if you get here you have connected to the WiFi    
          Serial.println("connected...yeey :)");
        }
        currentMillis = millis() - buttonPressedMillis;
        if (currentMillis - previousMillis >= BLINK_INTERVAL) {
          ledState = (ledState == LOW) ? HIGH : LOW;
          digitalWrite(LED1_PIN, ledState);
          previousMillis = currentMillis;
        }
      }
    }
  }
}
