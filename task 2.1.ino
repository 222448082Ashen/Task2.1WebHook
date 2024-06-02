#include <WiFiNINA.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include "secrets.h"

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0;
unsigned long timerDelay = 60000;  // 60 seconds

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  ThingSpeak.begin(client);

  dht.begin();
}

void loop() {
  if (millis() - lastTime >= timerDelay) {
    lastTime = millis();
    
    // Read temperature and humidity from DHT11 sensor
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    // Print values to Serial Monitor
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");
    
    // Write values to ThingSpeak
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);
    if (t > 30){
      ThingSpeak.setField(3,1);
    }
    
    // Write to ThingSpeak
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}

