# Smart Elevator - Web Server

## In this project, we will explore a smart elevator system with a unique feature: floor access restoration. This system utilizes the ESP32 microcontroller and a web interface to provide convenient elevator control while addressing specific floor access limitations.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/3a0d6a62-2675-4556-afd8-a7809ca33d45)

+ *This project introduces a smart elevator system that offers several functionalities:*
  + Users can call the elevator and select their desired floor using a web interface accessible from a smartphone or any web browser device.
  + The system implements access control logic to restrict calls to specific floors (e.g., under maintenance or security restrictions).
  + Despite access limitations, the system prioritizes restoring access to previously restricted floors once conditions permit.
### Our proposed solution addresses the challenge of restricted floor access in an elevator system. It leverages a web interface for user convenience and integrates access control to enforce limitations. Importantly, the system offers the flexibility to restore access to previously restricted floors when necessary.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/c97be801-7ebf-47ff-bd0d-b9372772ff9e)![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/a1a2aa4b-22aa-49da-9953-7035e6a7db1a)

+ ESP32 microcontroller: The heart of the system, responsible for processing user requests, controlling relays, and interacting with the elevator control system.
+ Web Interface: A user-friendly interface accessible via a web browser, allowing users to call the elevator and select their desired floor.
+ Relay Control: Enables the ESP32 to control physical relays that can disable buttons or block access to specific floors if needed.
+ Elevator Control System: The existing system that manages elevator movement. Our project will modify its logic to skip restricted floors.
### This slide depicts the core components of our smart elevator system. The ESP32 microcontroller acts as the central processing unit, coordinating communication between the web interface, the relay control system, and the elevator control system. The web interface provides a convenient platform for user interaction. Relay control enforces access restrictions, and the elevator control system, with our modifications, will skip floors marked as restricted.

Start by building the circuit. Connect two LEDs to the ESP32 as shown in the following schematic diagram – one LED connected to `GPIO 26`, and the other to `GPIO 27`.
![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/09d78689-2e59-4aef-a059-37c86e1320bc)

### Here we provide the code that creates the ESP32 web server. Copy the following code to your Arduino IDE(VS Code), but don’t upload it yet. You need to make some changes to make it work for you.
```c++
// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
```

### You need to modify the following lines with your network credentials: SSID and password. The code is well commented on where you should make the changes.
```c++
// Replace with your network credentials
const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
```
Now, you can upload the code and and the web server will work straight away. Follow the next steps to upload code to the ESP32:

1) Plug your ESP32 board in your computer;

2) In the Arduino IDE select your board in `Tools > Board` (in our case we’re using the ESP32 DEVKIT DOIT board);
![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/95a10eff-97c2-4284-806f-47e4650b18f7)

3) Select the COM port in `Tools > Port`.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/fd6240a2-7303-40f1-ab79-0f4feb5f899b)

4) Press the Upload button in the Arduino IDE and wait a few seconds while the code compiles and uploads to your board.
![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/5c05916f-b679-4812-8a92-d30d3a5f0abf)

5) Wait for the “Done uploading” message.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/7b430c37-0429-4aa1-bb7d-1e29a6220769)

## Finding the ESP IP Address
After uploading the code, open the Serial Monitor at a baud rate of 115200.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/bb51f568-bf4a-4436-93d4-db88a2599d30)

Press the ESP32 EN button (reset). The ESP32 connects to Wi-Fi, and outputs the ESP IP address on the Serial Monitor. Copy that IP address, because you need it to access the ESP32 web server.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/0668f1da-2b08-4b02-a363-76d1f49e7454)

## Accessing the Web Server
To access the web server, open your browser, paste the ESP32 IP address, and you’ll see the following page. In our case it is `192.168.1.135`.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/f0e5d755-29c5-443f-b170-4b4a52af30d9)

If you take a look at the Serial Monitor, you can see what’s happening on the background. The ESP receives an HTTP request from a new client (in this case, your browser).

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/c9832198-be59-4d3d-8bfb-8d467bd26391)

+ User calls the elevator through the web interface.
+ The web interface transmits the floor request to the ESP32.
+ The ESP32 checks the floor against a database or programmed logic to determine access permission.
  + If access is granted, the ESP32 communicates with the elevator control system to direct the elevator to the requested floor.
  + If access is restricted, the user receives a notification on the web interface, and the elevator bypasses the restricted floor during its movement.
### This slide illustrates the system's functionality. When a user requests a floor through the web interface, the ESP32 verifies if access to that floor is permitted. Based on the access control logic, the ESP32 instructs the elevator control system to move the elevator to the desired floor or skip it if restricted. The web interface provides feedback to the user accordingly.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/092d6507-915f-4fc2-b7fc-be3e3e42fd9f)
