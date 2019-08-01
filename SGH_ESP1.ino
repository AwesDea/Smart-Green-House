//  SMART GREEN HOUSE ESP 1 LDR(photoresistor)

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
#define LDR_PIN      0              // Pin connected to the LDR. GPIO 0
#define TMP_PIN      15              // Pin connected to the TMP36. GPIO 15


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
/****************************** Feeds ***************************************/
// Setup a feeds for publishing.

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish pi_notif = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/notif");               //send notifications to RPi
Adafruit_MQTT_Publish pi_ldr = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/ldr");               //give RPi LDR notifications
Adafruit_MQTT_Publish pi_tmp = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/tmp");               //give RPi TMP notifications

/*************************** Sketch Code ************************************/

void MQTT_connect();
String ldr_value = "0";     // result of reading the analog pin


void setup() {
  Serial.begin(9600);

    Serial.println(F("SGH-ESP1-MQTT"));
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
  pi_notif.publish("SGH ESP1 WIFI connected.");
  
}

void loop() {
  
  /* Ensure the connection to the MQTT server is alive (this will make the first
  connection and automatically reconnect when disconnected).  See the MQTT_connect*/
  MQTT_connect();
  /* this is our 'wait for incoming subscription packets' busy subloop try to spend your time here*/

  delay(1000);                    // wait a little
  
  //**** LDR CODE****
  
  ldr_value = String(analogRead(LDR_PIN)); // read the value from the LDR
  Serial.println(ldr_value);      // print the value to the serial port
  char ldr_char_array[ldr_value.length() + 1];
  ldr_value.toCharArray(ldr_char_array, ldr_value.length() + 1 ); 
  pi_ldr.publish(ldr_char_array);


  //**** TMP CODE****

 //getting the voltage reading from the temperature sensor
 int reading = analogRead(TMP_PIN);  
 
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 
 // print out the voltage
 Serial.print(voltage); Serial.println(" volts");
 
 // now print out the temperature
 float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
 Serial.print(temperatureC); Serial.println(" degrees C");
 
  String tmp = String(temperatureC);
  char tmp_char_array[tmp.length() + 1];
  tmp.toCharArray(tmp_char_array, tmp.length() + 1 ); 
  pi_tmp.publish(tmp_char_array);

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
    pi_notif.publish("ESP 1 MQTT connected.");
}

