# BuzzAlert

BuzzAlert - IoT University Project for the IoT 2022 Course @ Sapienza University of Rome.

| **Name and Surname** | **Linkedin** | **GitHub** |
|:--------------------:| :---: | :---: |
|  `Antonio Grieco `   | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/LogoIn.png)](https://www.linkedin.com/in/AntonioGrieco96) | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/GitHubLogo.png)](https://github.com/AGrieco96) |



## Introduction

`BuzzAlert` Project.

The BuzzAler project is an innovative alarm system designed to enhance security and provide remote access and control capabilities. With its integrated ultrasonic sensor, buzzer, Servo motor, RGB LED, vibration detection sensor and Nucleo F401RE development board, BuzzAlert offers comprehensive monitoring and control of your door's security status. Leveraging cloud connectivity, users can remotely access and manage the system, ensuring peace of mind and heightened security for homes or offices.

## Architecture Design
![image](/path/to/your/image.png) (QUA CI METTI FOTO ARCHITETTURA)

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
<img src="docs/sharedpictures/RiotOsLogo.png" width="200" align="right"/>
(QUA LOGO RIOT OS)

* ### RIOT OS

  RIOT powers the Internet of Things like Linux powers the Internet. RIOT is an open-source microkernel-based operating system, designed to match the requirements of Internet of Things (IoT) devices and other embedded devices. These requirements include a very low memory footprint (on the order of a few kilobytes), high energy efficiency, real-time capabilities, support most low-power IoT devices, microcontroller architectures (32-bit, 16-bit, 8-bit), and external devices. RIOT aims to implement all relevant open standards supporting an Internet of Things that is connected, secure, durable & privacy-friendly.

  A good high-level overview can be found in the article [RIOT: An Open Source Operating System for Low-End Embedded Devices in the IoT](https://www.riot-os.org/assets/pdfs/riot-ieeeiotjournal-2018.pdf) (IEEE Internet of Things Journal, December 2018).

## Network 

The following image represents a description of the entire network system that we can find within the project.

![image](/docs/sharedpictures/NetworkArchitecture.png) (QUA FOTO DELLA RETE)


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

This project has two different principles, detecting intruders and locking a door, the working principle is very easy : there is an ***(Ultrasonic sensor)*** that senses the distance, if the last two reads are shows a difference that is greater than 20CM it means that the door is Closing/Opening, at this point if the alarm part of the project is in state ***(ON)*** the ***(Buzzer)*** will be triggered emitting a strong noise in order to alert the owner,also the ***(LED)*** will flash to provide a visible feedback, this change will also be visible on the ***(WebPage)*** in the ***(STATE)*** section.
The second part of the project is a SmartLock, using a ***(Servo Motor)*** the locker on the door can be ***(Released/Closed)***, when the locker is in state Closed the ***(Vibration Sensor)*** is actived to detect possible intruders, trying to destroy the door.

More in detail:

  **(Activation)**
    In this phase all the sensors are initialized, showing up in the console if some errors occurs, in case of wrong wiring or other software problems, the the board will subscribe to the topic in which it will publish the relevant data, through a transparent bridge written in python, all the tecnical stuff regarding this will be discussed later.
    When the initialization phase is completed we have two different configuations, if the system is in state `SmartLock` the Vibration Sensor its activated to detect intruders, if it sense a vibration level greater than the threeshold, that has been chosen according to some test performed, and it is the upper bound needed if we don't want to loose any information, a signal will be triggered and the buzzer will start to produce noise, also the led its triggered and a visual feedback is given on the WebApp.
    If we are in state `SmartAlarm` the board will enter a loop that senses the distance every 2 seconds, this threeshold has been chosen according to some test performed, and it is the upper bound needed if we don't want to loose any information, the value chosen for the Ultrasonic Sensor has been choosen in the same way as the Vibration Sensor one.
    

An important aspect is the fact that only relevant data are sent to the Cloud, since the board it's capable of some basic computation, when the Ultrasonic Sensor detects a change in the distance it computes locally the distance and will send the information to the Cloud only if there is a change in the State of the application, not only in the Distances.
This is done to avoid sending Data that are not relevant preserving the network and also to minimize congestion, avoiding collision, that can create delay and wrong information to the final user.

### General Description

Data management and sending within this project is done using the MQTT protocol. We have the board connected to the mosquitto.rsmb server and our AwS IoTCore service which are connected through a transparent bridge that we wrote in python. We therefore have our mqtt broker which we have subscribed to the "topic_data" topic and our AwS IoTCore broker which is instead subscribed to the "topic_board" topic.

The board builds the information to send and publishes it on the topic: *topic_data*, the transparent bridge captures this message and sends it by invoking a lambda function on a topic to which we have subscribed the AwSIoTCore and then save it on a table in DynamoDB .
An important aspect of this operation is the Policy that we associate with our object in AwSIoTCore, here is an [example of policy](/IoTCore_thing/windforme-Policy).  ****ADD LINK TO POLICY

Two-way communication has been implemented, so from our web application we can communicate with the board.
For the moment the only functionality that has been implemented is the sending of a message that changes the system mode of the entire application. The message generated by the web application is published through a lambda function on the topic: *topic_board* . The transparent bridge is activated to carry the message published on the broker of the board, in order to trigger the callback function of the mqtt protocol.  The message is then processed in order to extract the command to be executed.

Another important aspect is the assignment of IAM Policy for lambda functions, which depending on the type of service they offer need permissions.

Also, it is possible to trigger from the WebApplication the synchronization of information retrieval whenever the board sends a message to the database. This clamping was handled by taking into account the latencies that the message undergoes in sending and receiving.

### Network performance

Using a packet analyzer tool it is possible to know the weight of the packets.

So in the first case we have a message that is lees than 37bytes. In the second case in the worst case since we need to send one of [ auto - on - off ] his length is maximum 5bytes . In both cases we have a very short latency, one caused from when the sensor register a new value and the other one from when a button is clicked on the web app to when the system acts. This latency are short enough to not affect the usability of the system.

## Data Processing

### Edge Computing

Then there are two process that are running fully on the device itself : 
* the process of the automatic system which need to compare the value read from the sensor with the threshold.
* trigger logic of the motor - once is received the message command from the webapp.  

In this way the network latency does not influence the capability of the device.

### Cloud Computing

The Cloud part of the project is performed by AWS, that receives the payload containing **"Read,PastRead and State"** on the MQTT broker and through a custom RULE saves them in **"Black_Table"**, that is the DynamoDB table used.
![image](/docs/sharedpictures/AwSIoTRule.png) (QUA FOTO DELLA RULE CHE METTE I DATI IN SQL SU DYNAMODB)

## Setup & Run 

### Configuration AWS Environment
* **AWS IoTCore**.  
  You need to setup a thing on this services then you need to download some certification, they need to look like the ones that are in IoTCore_thing folder. The most important thing is the Policy, in order to use this project you should have a Policy like [windforme-Policy](/IoTCore_thing/windforme-Policy).  
  Then you need to construct a messagging rules like the one described before in the cloudcomputing part in order to write in the dynamodb table the data received from the board.
* **Lambda function**.  
  Write two lambda functions such as [publish_to_iotcore](/LambdaFunctions/publish_to_iotcore.py) and [read_data](/LambdaFunctions/read_data.py).  
  If they are created on aws lambda, pay attention to the role, you probably need to use the IAM management console to customize them. 
  In fact, for the first lambda function, I applied the AWSIoTFullAccess policy to the specific associated role, then for the second lambda function, however, I modified the policy already attached to the associated role and added write permission on dynamodb services.
* **DynamoDB**.  
  Simply creates a table called "your_table_name," the name of which must be the same as written in the read_data lambda function.
* **AWSAmplify** ***not strictly necessary***.  
  You can hosts the entire webApp folder on the AWSAmplify service. 
  By the way, you can test the application on localhost.

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

You can read more specific information about the iAlarm project in this [blog post](https://www.yourblogpostlink.com).

## Demo video

Watch a demo presentation of the iAlarm project [here](https://www.yourdemovideolink.com).
