#include <secret.h>
#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#define WIFI_AP_NAME "ESP32-TempIoT"
#define WIFI_AP_PASSWORD "celcius273"
#define RESET_BUTTON_PIN 18
#define DEBOUNCE_TIME 50
#define DHTPIN 5
#define DHTTYPE DHT22
#define AWS_IOT_PUBLISH_TOPIC "esp32/temp"

// Variables will change:
int ResetLastSteadyState = LOW;
int ResetLastFlickerableState = LOW;
int Reset_Button_State = 0;

unsigned long ResetLastDebounceTime = 0;
unsigned long lastMillis;
unsigned long epochTime;

bool isConnected = false;

const char* ntpServer = "0.dk.pool.ntp.org";

DHT dht(DHTPIN, DHTTYPE);
WiFiManager wifiManager;
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

unsigned long long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  long long milliseconds = static_cast<long long>(now) * 1000;
  return milliseconds;
}

void resetSystem(){
  Serial.println("Reset System ...");
  wifiManager.resetSettings();
  delay(2000);
  Serial.println("Reboot System ...");
  delay(2000);
  ESP.restart();
}

void connectAWS(){
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);
    client.setServer(AWS_IOT_ENDPOINT, 8883);
    client.connect(THINGNAME);

    while (!client.connected()) {
      Serial.println("Connecting to MQTT....Retry");
      client.connect(THINGNAME);
      delay(5000);
    }
  Serial.println("MQTT Connected");
}

double getTemp(){
    double temp = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("ºC ");
    return temp;
}

double getHumi(){
    double humi = dht.readHumidity();
    Serial.print("Humidity: ");
    Serial.println(humi);
    return humi;
}

void publishMessage()
{
    StaticJsonDocument<200> doc;
    doc["time"] = getTime();
    doc["temp"] = getTemp();
    doc["humi"] = getHumi();
    doc["hostname"] = THINGNAME;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup() {
    Serial.begin(921600);
    dht.begin();
    wifiManager.setMinimumSignalQuality(50);
    wifiManager.autoConnect(WIFI_AP_NAME, WIFI_AP_PASSWORD);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RESET_BUTTON_PIN, INPUT);
    configTime(0, 0, ntpServer);
    connectAWS();
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
      publishMessage();
    }
}
