#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <WiFiManager.h>

#define WIFI_AP_NAME "ESP32-TempIoT"
#define WIFI_AP_PASSWORD "celcius273"
#define RESET_BUTTON_PIN 18
#define DEBOUNCE_TIME 50
#define DHTPIN 5
#define DHTTYPE DHT11

// Variables will change:
int ResetLastSteadyState = LOW;
int ResetLastFlickerableState = LOW;
int Reset_Button_State = 0;

unsigned long ResetLastDebounceTime = 0;
unsigned long lastMillis;

bool isConnected = false;

DHT dht(DHTPIN, DHTTYPE);
WiFiManager wifiManager;

void resetSystem(){
  Serial.println("Reset System ...");
  wifiManager.resetSettings();
  delay(2000);
  Serial.println("Reboot System ...");
  delay(2000);
  ESP.restart();
}

void getTemp(){
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("ºC ");
    Serial.print("Humidity: ");
    Serial.println(humi);
}

void setup() {
    Serial.begin(921600);
    dht.begin();
    wifiManager.setMinimumSignalQuality(50);
    wifiManager.autoConnect(WIFI_AP_NAME, WIFI_AP_PASSWORD);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RESET_BUTTON_PIN, INPUT);
}

void loop() {
    Reset_Button_State = digitalRead(RESET_BUTTON_PIN);

    String WiFiStatus = wifiManager.getWLStatusString().c_str();

    if (wifiManager.getWLStatusString() != "Unknown" && !isConnected) {
      Serial.println("Connected");
      Serial.println(wifiManager.getWiFiSSID());
      digitalWrite(LED_BUILTIN, HIGH);
      isConnected = true;
    }

    // Reset Button - Start
    if (Reset_Button_State != ResetLastFlickerableState) {
      ResetLastDebounceTime = millis();
      ResetLastFlickerableState = Reset_Button_State;
    }

    if ((millis() - ResetLastDebounceTime) > DEBOUNCE_TIME) {
      if (ResetLastSteadyState == HIGH && Reset_Button_State == LOW) {
        Serial.println("The Reset button is pressed");
        resetSystem();
      }
      ResetLastSteadyState = Reset_Button_State;
    }
    // Reset Button - Stop
    if (millis() - lastMillis >= 5*1000UL) {
      lastMillis = millis();
      getTemp();
    }
}
