#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

#define RST_PIN         22
#define SS_PIN          5
#define RELAY_PIN       21

const char* ssid = "Galaxy";
const char* password = "vgtm78923";

const char* serverName = "http://16.16.126.15:5000/check_uid"; // Update to your EC2 instance IP

MFRC522 mfrc522(SS_PIN, RST_PIN);

bool checkUID(String uid);

void printLocalTime();

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set time zone to Uzbekistan Time (UTC+5)
  configTzTime("UZT-5", "pool.ntp.org");

  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Time is set");

  printLocalTime();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += mfrc522.uid.uidByte[i] < 0x10 ? "0" : "";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  Serial.println("UID: " + uid);

  if (checkUID(uid)) {
    digitalWrite(RELAY_PIN, HIGH);
    delay(20000);  // Relay stays on for 20 seconds
    digitalWrite(RELAY_PIN, LOW);
  }
  mfrc522.PICC_HaltA();
}

bool checkUID(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(200);
    doc["uid"] = uid;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);

      DynamicJsonDocument responseDoc(200);
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        bool valid = responseDoc["valid"];
        if (valid) {
          const char* name = responseDoc["name"];
          const char* time = responseDoc["time"];
          Serial.print("User: ");
          Serial.println(name);
          Serial.print("Time: ");
          Serial.println(time);
          return true;
        }
      } else {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  return false;
}

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
