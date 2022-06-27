#include <ezButton.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define BUTTON_PIN         13
#define LED1_PIN           14
#define LED2_PIN           15
#define LED3_PIN           33
#define TEMP1_PIN          2
#define TEMP2_PIN          12
#define LONG_PRESS_TIME    1000
#define TIMEOUT_WIFI_SETUP 180000
#define BLINK_INTERVAL     500

bool wifiConnected = false;
bool wifiPortalRunning = false;
unsigned long wifiPortalLaunchedTime = 0;
bool buttonLongPressed = false;
unsigned long buttonLongPressedTime = 0;
TaskHandle_t Task0;
ezButton button(BUTTON_PIN);
WiFiManager wm;
OneWire oneWire1(TEMP1_PIN);
OneWire oneWire2(TEMP2_PIN);
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
float temp1;
float temp2;

void Task0code( void * pvParameters ){
  int ledState = LOW;
  unsigned long previousTime = 0;
  unsigned long currentTime = 0;
  unsigned long pressedTime  = 0;
  unsigned long releasedTime = 0;
  long pressDuration = 0;
  long testDuration = 0;
  long sinceLastLongPressDuration = 0;
  for (;;) {
    button.loop();
    delay(1);

    if (wifiPortalRunning) {
      currentTime = millis();
      testDuration = currentTime - previousTime;
      if (testDuration >= BLINK_INTERVAL) {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(LED3_PIN, ledState);
        previousTime = currentTime;
      }
    }
    else {
      if (wifiConnected) {
        digitalWrite(LED3_PIN, HIGH);
      }
      else {
        digitalWrite(LED3_PIN, LOW);
      }
    }

    if (millis() > LONG_PRESS_TIME) {
      if (button.isPressed()) {
        pressedTime = millis();
      }

      if (button.isReleased()) {
        releasedTime = millis();
        pressDuration = releasedTime - pressedTime;
        sinceLastLongPressDuration = releasedTime - buttonLongPressedTime;
        if ((pressedTime >= 10) && (pressDuration >= LONG_PRESS_TIME)) {
          if ((!buttonLongPressed) && (sinceLastLongPressDuration >= LONG_PRESS_TIME)) {
            buttonLongPressedTime = millis();
            buttonLongPressed = true;
          }
        }
        else {
          buttonLongPressed = false;
        }
      }
    }
  }
}

void configModeCallback (WiFiManager *myWiFiManager) {
  wifiPortalRunning = true;
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  wm.setConfigPortalBlocking(false);
  wm.setAPCallback(configModeCallback);
  wm.autoConnect("ESP32_Portal");
  pinMode(LED1_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED2_PIN, LOW);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED3_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.setDebounceTime(100); // set debounce time to 50 milliseconds
  sensor1.begin();
  sensor2.begin();
  xTaskCreatePinnedToCore(Task0code, "Task0", 10000, NULL, 1, &Task0, 0);
}

void loop() {
  delay(500);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
  }
  else {
    wifiConnected = false;
  }

  if (wm.getConfigPortalActive()) {
    wifiPortalRunning = true;
  }
  else {
    wifiPortalRunning = false;
  }

  if ((buttonLongPressed) && (!wifiPortalRunning)) {
    wm.startConfigPortal("ESP32_Portal");
    wifiPortalRunning = true;
    wifiPortalLaunchedTime = millis();
  }

  if (wifiPortalRunning) {
    wm.process();
    if ((!buttonLongPressed) || ((wifiPortalLaunchedTime >= 10) && (millis() - wifiPortalLaunchedTime >= TIMEOUT_WIFI_SETUP))) {
      wm.stopConfigPortal();
      wifiPortalRunning = false;
    }
  }

  sensor1.requestTemperatures();
  sensor2.requestTemperatures();
  temp1 = sensor1.getTempCByIndex(0);
  temp2 = sensor2.getTempCByIndex(0);

  if (temp1 != DEVICE_DISCONNECTED_C) {
    Serial.print("Temp1: ");
    Serial.println(temp1);
    digitalWrite(LED1_PIN, HIGH);
  }
  if (temp2 != DEVICE_DISCONNECTED_C) {
    Serial.print("Temp2: ");
    Serial.println(temp2);
    digitalWrite(LED2_PIN, HIGH);
  }
}
