# Smart Elevator
![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/2174fcc2-fcd0-46e8-aa5f-3fd5bacc11df)

## In this project, we will explore a smart elevator system with a unique feature: floor access restoration. This system utilizes the ESP32 microcontroller and a web interface to provide convenient elevator control while addressing specific floor access limitations.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/22f7df65-0f67-41a2-88b2-b6b9cd352903)

+ Conventional elevator systems provide access to all floors.
+ In some scenarios, access to certain floors might be restricted due to maintenance, security concerns, or other reasons.
+ This can cause inconvenience for users who need to reach those floors.
### This picture addresses a situation where a standard elevator system requires restricted access to specific floors. This could be due to maintenance work on those floors, security protocols, or other reasons. A conventional system would require users to use alternative means to reach the restricted floors, causing inconvenience.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/3a0d6a62-2675-4556-afd8-a7809ca33d45)

+ *This project introduces a smart elevator system that offers several functionalities:*
  + Users can call the elevator and select their desired floor using a web interface accessible from a smartphone or any web browser device.
  + The system implements access control logic to restrict calls to specific floors (e.g., under maintenance or security restrictions).
  + Despite access limitations, the system prioritizes restoring access to previously restricted floors once conditions permit.
### Our proposed solution addresses the challenge of restricted floor access in an elevator system. It leverages a web interface for user convenience and integrates access control to enforce limitations. Importantly, the system offers the flexibility to restore access to previously restricted floors when necessary.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/c97be801-7ebf-47ff-bd0d-b9372772ff9e)

+ ESP32 microcontroller: The heart of the system, responsible for processing user requests, controlling relays, and interacting with the elevator control system.
+ Web Interface: A user-friendly interface accessible via a web browser, allowing users to call the elevator and select their desired floor.
+ Relay Control: Enables the ESP32 to control physical relays that can disable buttons or block access to specific floors if needed.
+ Elevator Control System: The existing system that manages elevator movement. Our project will modify its logic to skip restricted floors.
### This slide depicts the core components of our smart elevator system. The ESP32 microcontroller acts as the central processing unit, coordinating communication between the web interface, the relay control system, and the elevator control system. The web interface provides a convenient platform for user interaction. Relay control enforces access restrictions, and the elevator control system, with our modifications, will skip floors marked as restricted.

![](https://github.com/Su13043/Smart-Elevator-System-with-Floor-Access-Restoration/assets/145110827/092d6507-915f-4fc2-b7fc-be3e3e42fd9f)

+ User calls the elevator through the web interface.
+ The web interface transmits the floor request to the ESP32.
+ The ESP32 checks the floor against a database or programmed logic to determine access permission.
  + If access is granted, the ESP32 communicates with the elevator control system to direct the elevator to the requested floor.
  + If access is restricted, the user receives a notification on the web interface, and the elevator bypasses the restricted floor during its movement.
### This slide illustrates the system's functionality. When a user requests a floor through the web interface, the ESP32 verifies if access to that floor is permitted. Based on the access control logic, the ESP32 instructs the elevator control system to move the elevator to the desired floor or skip it if restricted. The web interface provides feedback to the user accordingly.
