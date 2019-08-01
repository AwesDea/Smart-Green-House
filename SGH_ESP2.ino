//  SMART GREEN HOUSE ESP 2 Valve & YL69(moisture)

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
#define VALVE_PIN      0              // Pin connected to the valve. GPIO 0
#define SOIL_PIN      15              // Pin connected to the soil. GPIO 15


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
/****************************** Feeds ***************************************/
// Setup a feeds for publishing.

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish pi_notif = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/notif");               //send notifications to RPi
Adafruit_MQTT_Publish pi_soil = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/soil");               //give RPi soil notifications
Adafruit_MQTT_Publish pi_valve = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/valve");               //give RPi valve notifications

// Setup a feed for subscribing to changes.
Adafruit_MQTT_Subscribe esp_valve = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/esp2/valve");            // get messages for valve

/*************************** Sketch Code ************************************/

void MQTT_connect();


void setup() {
  
  Serial.begin(9600);

  Serial.println(F("SGH-ESP2-MQTT"));
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
  pi_notif.publish("SGH ESP2 WIFI connected.");
  
  // Setup MQTT subscription for feeds.
  mqtt.subscribe(&esp_valve);

  // Setup pins    
  pinMode(VALVE_PIN,OUTPUT);
  digitalWrite(VALVE_PIN, LOW);
}

void loop() {
  
  /* Ensure the connection to the MQTT server is alive (this will make the first
  connection and automatically reconnect when disconnected).  See the MQTT_connect*/
  MQTT_connect();
  /* this is our 'wait for incoming subscription packets' busy subloop try to spend your time here*/

  int soil_value = analogRead(SOIL_PIN);


  // Logic Loop that sets the required LED to ON
  if (soil_value >= 1000) (digitalWrite(VALVE_PIN, HIGH));
  else if ((soil_value <= 999) && (soil_value >=901)) (digitalWrite(VALVE_PIN, LOW));
  else if (soil_value <= 900) (digitalWrite(VALVE_PIN, LOW));
  else ;

  // Prints the condition of soil.  Dry, Wet or Perfect
  if (soil_value >= 1000) (Serial.print("SOIL IS TOO DRY!!!!!    "));
  else if ((soil_value <= 999) && (soil_value >=901)) (Serial.print("SOIL IS PERFECT!!!!!    "));
  else if (soil_value <= 900) (Serial.print("SOIL IS TOO WET!!!!!    "));
  else;
  
  // print out the value you read:
  Serial.print("Soil Humidity is: ");
  Serial.println(soil_value);
  delay(500);        // delay in between reads for stability

  String soil_string = String(soil_value);
  char soil_string_array[soil_string.length() + 1];
  
  soil_string.toCharArray(soil_string_array, soil_string.length() + 1 ); 
  pi_soil.publish(soil_string_array);


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
    pi_notif.publish("ESP 2 MQTT connected.");
}
