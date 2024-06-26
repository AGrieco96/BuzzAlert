import paho.mqtt.client as mqtt
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from AWSIoTPythonSDK.exception.AWSIoTExceptions import connectError
import json
import paho.mqtt.publish as publish
from datetime import datetime

#AWS Paramater Init
AWS_HOST = "a3b5fvirnosinx-ats.iot.eu-west-1.amazonaws.com" #can be found as parameter '--endpoint' in the last line of the file start.sh

                    # Change with your absolute path 
AWS_ROOT_CAP_PATH = "/home/nardo/Desktop/BlackProject/FinalProject/Bridge/root-CA.crt"
AWS_PRIVATE_KEY_PATH = "/home/nardo/Desktop/BlackProject/FinalProject/Bridge/blackProject.private.key"
AWS_CERTIFICATE_PATH = "/home/nardo/Desktop/BlackProject/FinalProject/Bridge/blackProject.cert.pem"
AWS_PORT = 8883

# MQTT broker details
BROKER_ADDRESS = "localhost"
BROKER_PORT = 1886
# Topic to subscribe to

#define MQTT_TOPIC_INT      "topic_board"
#define MQTT_TOPIC_EXT      "topic_data"
TOPIC_INT = "topic_board"
TOPIC_EXT = "topic_data"
TOPIC_TO_AWS = "topic_temp"



def myCallback(client, userdata, message):
    print("Received message on topic:", message.topic)
    print("Message:", message.payload)
    #publish.single(TOPIC,payload=message.payload, hostname="localhost",port="1886")
    mqttClient.publish(TOPIC_INT,message.payload)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    if rc == 0:
        print("Subscribe to topic : "+TOPIC_EXT)
        client.subscribe(TOPIC_EXT)
    else:
        print("Connection failed. Return code =", rc)

def on_message(client, userdata, msg):
    print("Received message:", msg.topic, msg.payload)
    message = str(msg.payload)[2:-1]
    splitted = message.split(":")
    message_out= {}
    message_out['Distance'] = str(splitted[0])
    message_out['PastDistance'] = str(splitted[1])
    message_out['State'] = str(splitted[2])
    print(message_out)
    messageJson=json.dumps(message_out)
    print("Json inviato  :   "+messageJson)
    #Publishing message 
    
    myAWSIoTMQTTClient.publish(TOPIC_TO_AWS, messageJson, 1)


print("AWS Client connection")
myAWSIoTMQTTClient = None
myAWSIoTMQTTClient = AWSIoTMQTTClient("windforme")
myAWSIoTMQTTClient.configureEndpoint(AWS_HOST, AWS_PORT)
myAWSIoTMQTTClient.configureCredentials(AWS_ROOT_CAP_PATH, AWS_PRIVATE_KEY_PATH, AWS_CERTIFICATE_PATH)
try:
    myAWSIoTMQTTClient.connect()
    print("Connected to AWS IoT MQTT broker")
except connectError as e:
    print("Connection to AWS IoT MQTT broker failed:", str(e))



print("MQTT Client connection")
# Create MQTT client instance
mqttClient = mqtt.Client()
# Set up callbacks
mqttClient.on_connect = on_connect
mqttClient.on_message = on_message

print("AWSClient Subscription")
myAWSIoTMQTTClient.subscribe(TOPIC_INT,1,myCallback)


# Connect to MQTT broker
mqttClient.connect(BROKER_ADDRESS, BROKER_PORT , 60)
# Start the network loop to handle MQTT communication
mqttClient.loop_forever()
