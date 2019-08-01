//  SMART GREEN HOUSE ESP 3 Fan & servo

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       ""
#define WLAN_PASS       ""

#define MQTT_SERVER      "" // ip address of rpi
#define MQTT_PORT         1883
#define MQTT_USERNAME    ""
#define MQTT_PASSWORD         ""



//
/************ Global State ******************/
#define FAN_PIN      15             // Pin connected to the Fan and sets speed of the fan. GPIO 0
#define SERVO_PIN      0              // Pin connected to the Servo. GPIO 15


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
/****************************** Feeds ***************************************/
// Setup a feeds for publishing.

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish pi_notif = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/notif");               //send notifications to RPi
Adafruit_MQTT_Publish pi_fan = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/fan");               //give RPi fan notifications
Adafruit_MQTT_Publish pi_servo= Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/servo");               //give RPi servo notifications

// Setup a feed for subscribing to changes.
Adafruit_MQTT_Subscribe esp_fan = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/esp3/fan");            // get messages for fan
Adafruit_MQTT_Subscribe esp_servo = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/esp3/servo");            // get messages for servo

/*************************** Sketch Code ************************************/


void set_motors_state(String motor,int state);

void MQTT_connect();


void setup() {
  
  Serial.begin(9600);

  Serial.println(F("SGH-ESP3-MQTT"));
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  pi_notif.publish("SGH ESP3 WIFI connected.");
  
  // Setup MQTT subscription for feeds.
  mqtt.subscribe(&esp_fan);
  mqtt.subscribe(&esp_servo);

  // Setup Pins
  pinMode(FAN_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);


}

void loop() {
  
  /* Ensure the connection to the MQTT server is alive (this will make the first
  connection and automatically reconnect when disconnected).  See the MQTT_connect*/
  MQTT_connect();
  /* this is our 'wait for incoming subscription packets' busy subloop try to spend your time here*/
  
// Here its read the subscription
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription())) {
    if (subscription == &esp_fan) {
      char *message = (char *)esp_fan.lastread;
      Serial.print(F("Got: "));
      Serial.println(message);
      
      // Check if the message was off, low, medium or high.
      if (strncmp(message, "off", 3) == 0) {
        // Turn the fan off
        set_motors_state("fan",0);
      }
      
      else if (strncmp(message, "low", 3) == 0) {
        // Turn the fan to low speed
        set_motors_state("fan",1);
      }
      
      else if (strncmp(message, "medium", 6) == 0) {
        // Turn the fan to medium speed
        set_motors_state("fan",2);
      }
      
      else if (strncmp(message, "high", 4) == 0) {
        // Turn the fan to high speed
        set_motors_state("fan",3);
      }
    }

    else if (subscription == &esp_servo) {
      char *message = (char *)esp_fan.lastread;
      Serial.print(F("Got: "));
      Serial.println(message);
      
      // Check if the message was off, low, medium or high.
      if (strncmp(message, "off", 3) == 0) {
        // Open Servo
        set_motors_state("servo",0);
      }
      
      else if (strncmp(message, "on", 3) == 0) {
        // Close Servo
        set_motors_state("servo",1);
      }
    } 
  }
}


//function for setting speed of fan
void set_motors_state(String motor, int state) {

  if (motor == "fan"){
  
    if (state == 0) {
      analogWrite(FAN_PIN, 0); // Send PWM signal to Fan
      pi_fan.publish("fan turned OFF");
    }
    
    else if (state == 1) {
      analogWrite(FAN_PIN, 130); // Send PWM signal to Fan
      pi_fan.publish("esp_fan turned LOW");
    }
    
    else if (state == 2) {
      analogWrite(FAN_PIN, 185); // Send PWM signal to Fan
      pi_fan.publish("esp_fan turned MEDIUM");
    }
    
    else if (state == 3) {
      analogWrite(FAN_PIN, 255); // Send PWM signal to Fan
      pi_fan.publish("esp_fan turned HIGH");
    }
  }
  else if (motor == "servo"){
    
    if (state == 0) {
      //TODO edit voltage and delay
      analogWrite(SERVO_PIN, 100); // Send PWM signal to Servo
      delay(3000);
      analogWrite(SERVO_PIN, 0); // Send PWM signal to Servo
      pi_servo.publish("Servo opened");
    }
    else if (state == 1) {
      analogWrite(SERVO_PIN, 200); // Send PWM signal to Servo
      delay(3000);
      analogWrite(SERVO_PIN, 0); // Send PWM signal to Servo
      pi_servo.publish("Servo closed!");
    }
    
  }
}



void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
    pi_notif.publish("ESP 3 MQTT connected.");
}
