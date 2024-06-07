#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProFanUS.h"
#include "SinricProTemperaturesensor.h"
#include "DHTesp.h"

#define WIFI_SSID         "AdvanWifi"    
#define WIFI_PASS         "bayam2121"
#define APP_KEY           "9710fa0b-8555-495a-a26b-2756c094362a"
#define APP_SECRET        "2f0b08b0-6e51-4850-a3f3-75057d7189a0-c357d3d1-03f8-4d59-be7c-20fce3908952"
#define FAN_ID            "6662ab2f5d818a66fab5ab88"
#define TEMP_SENSOR_ID    "6662aa1b5d818a66fab5aadc"  // Replace with your temperature sensor device ID
#define DHT_PIN           D7  // Pin where the DHT22 is connected
#define BAUD_RATE         115200

const int buttonPin[] = {D2, D1, D4, D3}; // Pin untuk push buttons
const int relayPins[] = {D0, D5, D6};     // Pin untuk 3 channel relay

DHTesp dht;
unsigned long lastDHTReadTime = 0;
const unsigned long DHT_INTERVAL = 60000; // Read DHT sensor every 60 seconds

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
    setRelayState(device_state.fanSpeed);
  } else {
    setRelayState(0);
  }
  return true;
}

bool onRangeValue(const String &deviceId, int &rangeValue) {
  device_state.fanSpeed = rangeValue;
  Serial.printf("Fan speed changed to %d\r\n", device_state.fanSpeed);
  if (device_state.powerState) {
    setRelayState(device_state.fanSpeed);
  }
  return true;
}

bool onAdjustRangeValue(const String &deviceId, int &rangeValueDelta) {
  device_state.fanSpeed += rangeValueDelta;
  if (device_state.fanSpeed < 1) device_state.fanSpeed = 1;
  if (device_state.fanSpeed > 3) device_state.fanSpeed = 3;
  Serial.printf("Fan speed changed about %i to %d\r\n", rangeValueDelta, device_state.fanSpeed);

  if (device_state.powerState) {
    setRelayState(device_state.fanSpeed);
  }
  rangeValueDelta = device_state.fanSpeed;
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
  SinricProTemperaturesensor &myTempSensor = SinricPro[TEMP_SENSOR_ID];

  myFan.onPowerState(onPowerState);
  myFan.onRangeValue(onRangeValue);
  myFan.onAdjustRangeValue(onAdjustRangeValue);

  SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();

  for (int i = 0; i < 3; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  dht.setup(DHT_PIN, DHTesp::DHT22);
}

void loop() {
  SinricPro.handle();

  for (int i = 0; i < 4; i++) {
    if (digitalRead(buttonPin[i]) == LOW) {
      delay(50);
      if (digitalRead(buttonPin[i]) == LOW) {
        if (i == 3) {
          bool state = false;
          onPowerState(FAN_ID, state);
        } else {
          bool state = true;
          onPowerState(FAN_ID, state);
          int speed = i + 1;
          onRangeValue(FAN_ID, speed);
        }
        while (digitalRead(buttonPin[i]) == LOW) {
          delay(10);
        }
      }
    }
  }

  unsigned long currentTime = millis();
  if (currentTime - lastDHTReadTime >= DHT_INTERVAL) {
    lastDHTReadTime = currentTime;

    TempAndHumidity newValues = dht.getTempAndHumidity();
    if (dht.getStatus() == DHTesp::ERROR_NONE) {
      float temperature = newValues.temperature;
      float humidity = newValues.humidity;

      Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\r\n", temperature, humidity);

      SinricProTemperaturesensor &myTempSensor = SinricPro[TEMP_SENSOR_ID];
      myTempSensor.sendTemperatureEvent(temperature, humidity);
    } else {
      Serial.printf("DHT22 error: %s\r\n", dht.getStatusString());
    }
  }
}
