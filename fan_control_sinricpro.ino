#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProFanUS.h"

#define WIFI_SSID         "AdvanWifi"    
#define WIFI_PASS         "bayam2121"
#define APP_KEY           "9710fa0b-8555-495a-a26b-2756c094362a"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "2f0b08b0-6e51-4850-a3f3-75057d7189a0-c357d3d1-03f8-4d59-be7c-20fce3908952"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define FAN_ID            "6662ab2f5d818a66fab5ab88"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         115200                // Change baudrate to your need

const int buttonPin[] = {D2, D1, D4, D3}; // Pin untuk push buttons
const int relayPins[] = {D0, D5, D6};     // Pin untuk 3 channel relay

// we use a struct to store all states and values for our fan
// fanSpeed (1..3)
struct {
  bool powerState = false;
  int fanSpeed = 1;
} device_state;

void setRelayState(int powerLevel) {
  for (int j = 0; j < 3; j++) {
    digitalWrite(relayPins[j], (powerLevel == j + 1) ? LOW : HIGH);
  }
}

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Fan turned %s\r\n", state ? "on" : "off");
  device_state.powerState = state;
  if (state) {
    setRelayState(device_state.fanSpeed); // Turn on fan with last speed setting
  } else {
    setRelayState(0); // Turn off fan
  }
  return true; // request handled properly
}

// Fan rangeValue is from 1..3
bool onRangeValue(const String &deviceId, int &rangeValue) {
  device_state.fanSpeed = rangeValue;
  Serial.printf("Fan speed changed to %d\r\n", device_state.fanSpeed);
  if (device_state.powerState) {
    setRelayState(device_state.fanSpeed); // Change speed if fan is on
  }
  return true;
}

// Fan rangeValueDelta is from -3..+3
bool onAdjustRangeValue(const String &deviceId, int &rangeValueDelta) {
  device_state.fanSpeed += rangeValueDelta;
  if (device_state.fanSpeed < 1) device_state.fanSpeed = 1;
  if (device_state.fanSpeed > 3) device_state.fanSpeed = 3;
  Serial.printf("Fan speed changed about %i to %d\r\n", rangeValueDelta, device_state.fanSpeed);

  if (device_state.powerState) {
    setRelayState(device_state.fanSpeed); // Change speed if fan is on
  }
  rangeValueDelta = device_state.fanSpeed; // return absolute fan speed
  return true;
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  SinricProFanUS &myFan = SinricPro[FAN_ID];

  // set callback function to device
  myFan.onPowerState(onPowerState);
  myFan.onRangeValue(onRangeValue);
  myFan.onAdjustRangeValue(onAdjustRangeValue);

  // setup SinricPro
  SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();

  // Setup relay pins and button pins
  for (int i = 0; i < 3; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Turn off all relays at start
  }

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

void loop() {
  SinricPro.handle();

  // Check button states
  for (int i = 0; i < 4; i++) {
    if (digitalRead(buttonPin[i]) == LOW) {
      delay(50); // Debounce delay
      if (digitalRead(buttonPin[i]) == LOW) { // Confirm button press
        if (i == 3) { // If button 4 is pressed, turn off fan
          bool state = false;
          onPowerState(FAN_ID, state);
        } else { // If button 1, 2, or 3 is pressed, change speed
          bool state = true;
          onPowerState(FAN_ID, state);
          int speed = i + 1;
          onRangeValue(FAN_ID, speed);
        }
        while (digitalRead(buttonPin[i]) == LOW) { // Wait for button release
          delay(10);
        }
      }
    }
  }
}
