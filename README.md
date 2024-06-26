# BuzzAlert

BuzzAlert - IoT University Project for the IoT 2022 Course @ Sapienza University of Rome.

| **Name and Surname** | **Linkedin** | **GitHub** |
|:--------------------:| :---: | :---: |
|  `Antonio Grieco `   | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/LogoIn.png)](https://www.linkedin.com/in/AntonioGrieco96) | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/GitHubLogo.png)](https://github.com/AGrieco96) |



## Introduction

`BuzzAlert` Project.

The BuzzAler project is an innovative alarm system designed to enhance security and provide remote access and control capabilities. With its integrated ultrasonic sensor, buzzer, Servo motor, RGB LED, vibration detection sensor and Nucleo F401RE development board, BuzzAlert offers comprehensive monitoring and control of your door's security status. Leveraging cloud connectivity, users can remotely access and manage the system, ensuring peace of mind and heightened security for homes or offices.

## Architecture Design
![image](/asset/BlackSketch.png) 

## Hardware

* **Board : `Nucleo-f401re`**

  The Nucleo F401RE is a development board based on the STM32F401RE microcontroller. The STM32F401RE is a high-performance microcontroller from STMicroelectronics that is part of the STM32 family of 32-bit Arm Cortex-M4 microcontrollers. The board includes an ARM Cortex-M4 core microcontroller running at up to 84MHz with 512KB of flash memory and 96KB of RAM. It also includes a variety of peripherals, including multiple timers, communication interfaces (UART, SPI, I2C, CAN), and analog-to-digital converters.

* **Ultrasonic Sensor : `HC-SR04`**

  HC-SR04 is an ultrasonic ranging sensor that provides 2 cm to 400 cm non-contact measurement function. The ranging accuracy can reach 3mm, and the effectual angle is less than 15°. It can be powered from a 5V power supply.

  **Specifications:**
  - Working Voltage: DC 5V
  - Working Current: 15mA
  - Working Frequency: 40Hz
  - Max Range: 4m
  - Min Range: 2cm
  - Measuring Angle: 15 degrees
  - Trigger Input Signal: 10µS TTL pulse
  - Echo Output Signal: Input TTL lever signal and the range in proportion
  - Dimensions: 45 * 20 * 15mm

* **`Buzzer`**

  The buzzer is powered by 6V DC and has the following pin configuration:
  - Positive: Identified by (+) symbol or longer terminal lead. Can be powered by 6V DC.
  - Negative: Identified by short terminal lead. Typically connected to the ground of the circuit.

  **Features and Specifications:**
  - Rated Voltage: 6V DC
  - Operating Voltage: 4-8V DC
  - Rated Current: <30mA
  - Sound Type: Continuous Beep
  - Resonant Frequency: ~2300 Hz
  - Small and neat sealed package
  - Breadboard and Perf board friendly


* **`Medium Vibration Sensor`**
  
  The vibration sensor is used to detect if someone is trying to force the door, in an unintended way, a medium one is used beacuse after some experiments it was the one that acts better, providing more accurate results.

  
* **`Servo Motor S51`**
 
  The servo motor is used to open or close the lock installed on the door, based on the value returned by the vibration and ultrasonic sensor, or provided by the user using the web app. (the datasheet of the servo motor can be found here)

## Software
<img src="asset/riot_logo.png" width="200" align="right"/>

* ### RIOT OS

  RIOT powers the Internet of Things like Linux powers the Internet. RIOT is an open-source microkernel-based operating system, designed to match the requirements of Internet of Things (IoT) devices and other embedded devices. These requirements include a very low memory footprint (on the order of a few kilobytes), high energy efficiency, real-time capabilities, support most low-power IoT devices, microcontroller architectures (32-bit, 16-bit, 8-bit), and external devices. RIOT aims to implement all relevant open standards supporting an Internet of Things that is connected, secure, durable & privacy-friendly.

  A good high-level overview can be found in the article [RIOT: An Open Source Operating System for Low-End Embedded Devices in the IoT](https://www.riot-os.org/assets/pdfs/riot-ieeeiotjournal-2018.pdf) (IEEE Internet of Things Journal, December 2018).

## Network 

The following image represents a description of the entire network system that we can find within the project.

![image](/asset/network_architecture.jpg) 


* **Board**  
  RIOT provides the ethos_uhcpd tool to provide network connectivity to an MCU via the USB connection. In the case of the STM32 Nucleo-64 F401RE this is very useful as the development board does not provide a wireless network interface.
  The ethos_uhcpd tool builds on top of the serial interface, ethos (Ethernet Over Serial) and UHCP (micro Host Configuration Protocol). Ethos multiplexes serial data to separate ethernet packets from shell commands. The ethos_uhcpd tool will provide network connectivity through the TAP interface. After all connection , the board send messages through the MQTT protocol.
  
* **Protocol**  
  MQTT is a lightweight and flexible network protocol that uses a central message broker to coordinate communication among the peers of the network. Peers can interact with the broker by sending and receiving messages within specific topis. By organizing message exchanges into topics, devices can exchange data or commands with each other or in a group. This structure can allow the application developer to set up a hierarchy of devices with complex controlling schemes.

* **Mosquitto.rsmb**  
  Then the board is connected to mosquitto.rsmb : the Really Small Message Broker is a server implementation of the MQTT and MQTT-SN protocols. Any client that implements this protocol properly can use this server for sending and receiving messages.

* **Transparent Bridge**  
  A transparent bridge written in python that works as a bridge exchanging messages using MQTT between broker (In our case the broker : are the "board" and the AwS IoT Core). It reads messages from the local broker with "topic_data" and publishes them to AWS IoTCore on the same topic. It also reads messages from AWS IoT Core with "topic_board" and publishes them on the local broker with the same topic.

* **AwS Services**  
  This is the cloud part. Once the data arrives to AwS IoT Core, using a role they will be stored in a table created using the DynamoDB service. At this point using another AwS service, `Lambda Function` they are exposed on an HTTP endpoint, used by the WebApp to visualize the state of the alarm.
    
  To achieve this its needed to create an `IAM Role` in AwS, create some rules and some policies to respectively grant access and perform actions.

  AwS Amplify then is used to host our application. The WebApp is simple and clear, on the main page it displays the state, `Active/Inactive`, the story of all the intruders detected and other stuff, there is also a button to shutdown or powerup the system remotely and obviously another button to switch from SmartAlarm to SmartLock easily.


## Working Principle

This project integrates two main functionalities: intruder detection and door locking.

The system operates as follows:
- An ultrasonic sensor measures distance.
- If the difference between the last two readings is greater than 20 cm, it indicates the door is opening or closing.
- If the alarm is activated (in the "ON" state), a buzzer emits a loud noise and an LED flashes to alert the owner.
- The current state is displayed on a webpage in the "STATE" section.
- A servo motor controls the door lock, enabling locking and unlocking.
- A vibration sensor detects attempts to tamper with or break the door when locked.

#### `Activation Phase`

- All sensors are initialized, and any errors (e.g., wiring issues or software problems) are reported in the console.
- The board subscribes to a topic for publishing relevant data using a Python-based transparent bridge.

#### `SmartLock Mode`

- Activates the vibration sensor to detect intruders.
- Triggers an alert (buzzer and LED) if vibrations exceed a predetermined threshold.
- Provides visual feedback on a WebApp.

#### `SmartAlarm Mode`

- Continuously monitors distance using the ultrasonic sensor at 2-second intervals.
- Local computation and cloud data transmission occur only upon a change in application state, optimizing data transmission and network performance.

#### `Data Transmission Optimization`

- Sends only relevant data to the cloud, minimizing unnecessary transmission.
- Leverages local computation to optimize network performance and prevent delays.

This project prioritizes efficient intruder detection and door security through optimized sensor utilization and smart data transmission strategies.


### General Description

<img src="asset/aws_logo.png" width="200" align="right"/>

Data management and transmission in this project utilize the MQTT protocol. The board is connected to the mosquitto.rsmb server and AWS IoT Core service through a transparent Python-written bridge. The MQTT broker is subscribed to the topic_data topic, while the AWS IoT Core broker is subscribed to the topic_board topic.

#### Data Flow

- **Data Publishing from Board:**
  - The board compiles the information and publishes it on the `topic_data` topic.
  - A transparent bridge captures this message and invokes a Lambda function subscribed to AWS IoT Core.
  - The Lambda function stores the data in a DynamoDB table.

- **Web Application Communication:**
  - Two-way communication enables interaction from the web application to the board.
  - Currently, the web application can send messages to change the system mode of the application.
  - Messages generated by the web application are published via a Lambda function on the `topic_board` topic.
  - The transparent bridge facilitates message delivery to the board's broker, triggering MQTT protocol callbacks to process the commands.

#### Key Aspects

- **AWS IoT Core Policy:**
  - An essential part of the operation is the Policy associated with the AWS IoT Core object.
  - Example Policy: [Policy Link](AwS/thing/blackboard-Policy).

- **IAM Policies for Lambda Functions:**
  - Lambda functions are assigned IAM Policies based on the services they offer, ensuring appropriate permissions.

- **Information Synchronization:**
  - The Web Application can trigger information retrieval synchronization upon receiving messages from the board.
  - Latencies during message transmission and reception are considered for efficient synchronization.


### Network performance

Using a packet analyzer tool it is possible to capture the size of the packets, but since we are sending just an integer value it is trivial to analyze them in detail.

## Data Processing

### Edge Computing

Then there are two process that are running fully on the device itself : 
* Scanning the environement to detect movement or intruders.
* Trigger actuators to perform actions when needed.  

In this way the network latency does not influence the capability of the device.

### Cloud Computing

The Cloud part of the project is performed by AWS, that receives the payload,containing the state of the system, so if an alarm is triggered, on the MQTT broker and through a custom RULE saves them in **"Black_Table"**, that is the DynamoDB table used.
![image](/asset/write_black.jpg) 

## Setup & Run 

### AwS configuration

* **Create a Thing :**

  - Access the AWS IoT Core console.
  - Define a new Thing to represent your device.

* **Define IoT Policies:**

  - Create an IoT policy specifying permissions required by your Thing.
  - Grant necessary privileges like publish and subscribe permissions for IoT operations.

* **IoT Core Rules:**

  - Configure an IoT Core Rule to process incoming messages from your device.
  - Define actions (e.g., invoking Lambda functions) based on subscribed MQTT topics

* **Develop and Configure Lambda Functions:**

  - Create two Lambda functions:
    - [publish_to_IoTCore](/AwS/lambda/publish_to_IoTCore.py): Handles publishing messages to AWS IoT Core.
    - [read_value](/AwS/lambda/read_value.py): Processes incoming messages and stores data in DynamoDB.
  
* **IAM Role Configuration:**

 - Configure Lambda execution roles:

    - Attach AWSIoTFullAccess policy for publish_to_iotcore to enable publishing to IoT Core.
    - Modify the role's policy for read_data to grant DynamoDB write permissions (e.g., dynamodb:PutItem).

* **Create a DynamoDB Table:**

  - Define a DynamoDB table to store data received from the IoT device.
  - Deploy Web Application with AWS Amplify:

* **AWS Amplify**
  - Upload and configure contents of the webApp folder for deployment on Amplify.

### Mosquitto RSMB
* Download [mosquitto rsmb](!https://github.com/eclipse/mosquitto.rsmb).  
  ``` 
    cd mosquitto.rsmb/rsmb/src
    make
  ```
* Create a file ```whatever_name.conf``` . In this case we run the RSMB as MQTT and MQTT-SN capable broker, using port 1885 for MQTT-SN and 1886 for MQTT and enabling IPv6,  so we need to write this inside the whatever_name.conf file.
  ```
    # add some debug output
    trace_output protocol

    # listen for MQTT-SN traffic on UDP port 1885
    listener 1885 INADDR_ANY mqtts
    ipv6 true

    # listen to MQTT connections on tcp port 1886
    listener 1886 INADDR_ANY
    ipv6 true
  ```
* Start it.
  ```
    ./broker_mqtt whatever_name.conf
  ```
### Transparent bridge
* Go under the folder of bridge then install dependencies: 
  ```
    pip3 install -r requirements.txt
  ```
* Start transparent bridge.
  ```
    python3 transparent_bridge.py
  ```

### Flash the board

* You can launch directly the application by starting the script [start.sh](!/start.sh).  
  This script is very important, as it will set interfaces and global ip for the operation of the rsmb mosquitto server.
  You need to launch with sudo privilegies.
  ```
    sudo ./start.sh
  ```

## BlogPost

You can read more specific information about the BuzzAlert project in this [blog post](https://www.hackster.io/antonio-grieco/buzzalert-153abb).

## Demo video

Watch a demo presentation of the BuzzAlert project [here](https://youtu.be/Rqv_ebcaH50?si=-Pue708B209gE0Bn).
