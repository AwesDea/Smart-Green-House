# RPi  code
import time 
import RPi.GPIO as GPIO 
import paho.mqtt.client as mqtt
# Configuration:

# Initialize GPIOs

GPIO.setmode(GPIO.BCM)


# Setup callback functions that are  called when MQTT events happen like 
# connecting to the server or receiving data from a subscribed feed. 
def on_connect(client, userdata, flags, rc): 
   print("Connected with result code " + str(rc)) 
   # Subscribing in on_connect() means that if we lose the connection and 
   # reconnect then subscriptions will be renewed. 
   client.subscribe("/pi/notif") 
   client.subscribe("/pi/ldr")
   client.subscribe("/pi/tmp")
   client.subscribe("/pi/soil")
   client.subscribe("/pi/valve")
   client.subscribe("/pi/fan")
   client.subscribe("/pi/servo")

# The callback for when a PUBLISH message is received from the server. 
def on_message(client, userdata, msg):
    date = datetime.datetime.now()

    print(msg.topic + " " +str(msg.payload))
    
           
    if msg.topic == '/pi/ldr':
        pass
   
    elif msg.topic == '/pi/tmp':
        pass
        # TODO add temp code to change fan
        
    elif msg.topic == '/pi/soil':
        pass
        
    elif msg.topic == '/pi/valve':
        pass
    
    elif msg.topic == '/pi/fan':
        pass
    
    elif msg.topic == '/pi/servo':
        pass
    
    

# Create MQTT client and connect to localhost, i.e. the Raspberry Pi running 
# this script and the MQTT server. 
client = mqtt.Client() 
client.on_connect = on_connect 
client.on_message = on_message 
client.connect('localhost', 1883, 60) 
# Connect to the MQTT server and process messages in a background thread. 
client.loop_start() 
# Main loop to listen for button presses. 
print('Script is running, press Ctrl-C to quit...')



 #   client.publish('/esp3/servo', 'on')

        
