#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include <MFRC522.h>  // Include MFRC522 library

const char* ssid = "IoTLaB";        // Replace with your network SSID
const char* password = "iot303abab";  // Replace with your network password

const int servoPin = 18;
const int relayPin = 26;  // GPIO pin connected to relay module IN pin

Servo myServo;
Preferences preferences;

AsyncWebServer server(80);
int currentFloor = 0;

// RFID setup
#define RST_PIN         14         // Configurable, see typical pin layout above
#define SS_PIN          5          // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize the servo
  myServo.attach(servoPin);
  
  // Initialize the relay
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Initially turn off relay

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialize RFID reader
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  // Load the last saved floor from Preferences (flash memory)
  preferences.begin("elevator", false);
  currentFloor = preferences.getInt("lastFloor", 0);
  myServo.write(map(currentFloor, 0, 10, 0, 180)); // Assuming 10 floors and 0-180 degrees range

  // Initialize web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
  });

  server.on("/floor", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("number")) {
      int floor = request->getParam("number")->value().toInt();
      if (floor == 3 || floor == 4) {
        request->send(403, "text/plain", "Access to this floor is restricted.");
      } else {
        currentFloor = floor;
        myServo.write(map(currentFloor, 0, 10, 0, 180)); // Assuming 10 floors and 0-180 degrees range
        preferences.putInt("lastFloor", currentFloor); // Save current floor to flash
        request->send(200, "text/plain", "Floor changed to " + String(floor));
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // Start server
  server.begin();
}

void loop() {
  // Main loop does nothing, everything is handled by the web server

  // RFID card detection
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID of the card
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println(content.substring(1));

  if (content.substring(1) == "A1 9B D3 8F") // Replace with your RFID card UID
  {
    Serial.println("Authorized access");
    digitalWrite(relayPin, HIGH); // Open the relay (allow access to restricted floors)
    delay(5000); // Allow access for 5 seconds (adjust as needed)
    digitalWrite(relayPin, LOW); // Close the relay
  }
  else {
    Serial.println("Access denied");
  }
}
