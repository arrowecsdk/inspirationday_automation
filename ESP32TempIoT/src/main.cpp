#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(921600);
    dht.begin();
}

void loop() {
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("ºC ");
    Serial.print("Humidity: ");
    Serial.println(humi);
    delay(1000);
}