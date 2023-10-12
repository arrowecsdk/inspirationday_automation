#include <Arduino.h>
#include <Wire.h>
#include <WiFiManager.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define WIFI_AP_NAME "ESP32-Deploy"
#define WIFI_AP_PASSWORD "ansible64!"
#define RED_BUTTON_PIN 4
#define BLUE_BUTTON_PIN 5
#define RESET_BUTTON_PIN 18
#define DEBOUNCE_TIME 50

// Variables will change:
int RedLastSteadyState = LOW;
int RedLastFlickerableState = LOW;
int BlueLastSteadyState = LOW;
int BlueLastFlickerableState = LOW;
int ResetLastSteadyState = LOW;
int ResetLastFlickerableState = LOW;
int Red_Button_State = 0;
int Blue_Button_State = 0;
int Reset_Button_State = 0;

unsigned long RedLastDebounceTime = 0;
unsigned long BlueLastDebounceTime = 0;
unsigned long ResetLastDebounceTime = 0;

bool isConnected = false;

WiFiManager wifiManager;
LiquidCrystal_I2C lcd(0x27,16,2);

void display(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("System Ready...");
    lcd.setCursor(0, 1);
    lcd.print(wifiManager.getWiFiSSID());
}

void callTower(){
  if (isConnected){
      HTTPClient http;

      http.begin("http://jsonplaceholder.typicode.com/posts/1");
      http.addHeader("Content-Type", "text/plain");
      int httpResponseCode = http.PUT("Data");

      if (httpResponseCode > 0){
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.println("Error on HTTP PUT request");
        Serial.println(httpResponseCode);
      }
      http.end();
      delay(3000);
      display();
  }
}

void printToLcd(String color) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You Clicked me..");
  lcd.setCursor(4, 1);
  lcd.print(color);
}

void resetSystem(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reset System ...");
  Serial.println("Reset System ...");
  wifiManager.resetSettings();
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reboot System ...");
  Serial.println("Reboot System ...");
  delay(2000);
  ESP.restart();
}

void getChuck(){
  if (isConnected){
    HTTPClient http;

    http.begin("https://api.chucknorris.io/jokes/random");
    int httpCode = http.GET();

    if (httpCode > 0){
      String payload = http.getString();
      Serial.println(httpCode);

      char json[1000];
      payload.toCharArray(json, 1000);
      StaticJsonDocument<1000> doc;
      deserializeJson(doc, json);

      const char* joke = doc["value"];

      Serial.println(joke);
    }
    else {
      Serial.println("Error on HTTP request");
    }
    http.end();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Chuck Norris");
    lcd.setCursor(0, 1);
    lcd.print("Approved ...");
    delay(3000);
    display();
  }
}

void setup() {
  Serial.begin(921600);
  wifiManager.setMinimumSignalQuality(50);
  WiFiManagerParameter custom_text("<p>This is just a text paragraph</p>");
  wifiManager.addParameter(&custom_text);
  WiFiManagerParameter towerip("towerip", "Enter Tower IP", "", 50);
  WiFiManagerParameter toweroauth("toweroauth", "Enter Tower OAuth", "", 50);
  WiFiManagerParameter towerworkflowBlue("towerworkflow1", "Enter Tower Workflow Blue", "", 50);
  WiFiManagerParameter towerworkflowRed("towerworkflow2", "Enter Tower Workflow Red", "", 50);
  wifiManager.addParameter(&towerip);
  wifiManager.addParameter(&toweroauth);
  wifiManager.addParameter(&towerworkflowBlue);
  wifiManager.addParameter(&towerworkflowRed);
  wifiManager.autoConnect(WIFI_AP_NAME, WIFI_AP_PASSWORD);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RED_BUTTON_PIN, INPUT);
  pinMode(BLUE_BUTTON_PIN, INPUT);
  pinMode(RESET_BUTTON_PIN, INPUT);
  lcd.init();
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(1,0);
  lcd.print("System Ready...");

  Serial.println("System Ready...");

  Serial.println("Tower IP: ");
  Serial.println(towerip.getValue());
}

void loop() {
  Red_Button_State = digitalRead(RED_BUTTON_PIN);
  Blue_Button_State = digitalRead(BLUE_BUTTON_PIN);
  Reset_Button_State = digitalRead(RESET_BUTTON_PIN);

  String WiFiStatus = wifiManager.getWLStatusString().c_str();

  if (wifiManager.getWLStatusString() != "Unknown" && !isConnected) {
    Serial.println("Connected");
    digitalWrite(LED_BUILTIN, HIGH);
    isConnected = true;
    display();
  }

  // Red Button - Start
  if (Red_Button_State != RedLastFlickerableState) {
    RedLastDebounceTime = millis();
    RedLastFlickerableState = Red_Button_State;
  }

  if ((millis() - RedLastDebounceTime) > DEBOUNCE_TIME) {
    if (RedLastSteadyState == HIGH && Red_Button_State == LOW) {
      Serial.println("The Red button is pressed");
      printToLcd("Red");
      callTower();
    }
    RedLastSteadyState = Red_Button_State;
  }
  // Red Button - Stop

  // Blue Button - Start
  if (Blue_Button_State != BlueLastFlickerableState) {
    BlueLastDebounceTime = millis();
    BlueLastFlickerableState = Blue_Button_State;
  }

  if ((millis() - BlueLastDebounceTime) > DEBOUNCE_TIME) {
    if (BlueLastSteadyState == HIGH && Blue_Button_State == LOW) {
      Serial.println("The Blue button is pressed");
      printToLcd("Blue");
      getChuck();
    }
    BlueLastSteadyState = Blue_Button_State;
  }
  // Blue Button - Stop

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

}
