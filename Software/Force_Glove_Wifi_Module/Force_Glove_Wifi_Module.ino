/*
 *  File Name: Force_Glove_Wifi_Module.ino
 *  
 *  Application: HomeAssistant ESP8266 Wifi Modlue
 *  
 *  Description: THis code is for the Force Glove MQTT and WIFI
 *  access.  This will read in a digital pin and if the pin goes
 *  high it will send the activate signal via MQTT to HASS.
 *  Once the read digital pin goes low it will send the deactivate
 *  signal via MQTT to HASS.
 *  
 *  Author: M. Sperry - http://www.youtube.com/misperry
 *  Date: 06/23/2018
 *  Revision: 1.0
 *  
 *  
 */
 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
#define mqtt_user "" //enter your MQTT username
#define mqtt_password "" //enter your password
#define MQTT_SENSOR_TOPIC "ha/force_sense"

WiFiClient espClient;
PubSubClient client(espClient);
//int SwitchedPin = 12, LEDPin = 13, Override = 14, OverrideMem = 0;
bool HassSwitch = false;

int IndicationPin = 0;

void setup_wifi() {
 Serial.begin(115200);
  delay(2000);

  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());



  
}

void callback(char* topic, byte* payload, unsigned int length) {

}
 
 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESPBlindstl", mqtt_user, mqtt_password)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(IndicationPin,INPUT);
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  //check the indication pin to see if the curie nano has
  //indicated that we need to switch on/off
  if (digitalRead(IndicationPin) == 1 && HassSwitch == false)
  {
    client.publish(MQTT_SENSOR_TOPIC, "ON", true);
    Serial.println("State changed to: ON");
    HassSwitch = true;
    while (digitalRead(IndicationPin))
    {
      client.loop();
    }
  }
  else if (digitalRead(IndicationPin) == 1 && HassSwitch == true)
  {
    client.publish(MQTT_SENSOR_TOPIC, "OFF", true);
    Serial.println("State change to: OFF");
    HassSwitch = false;
    while (digitalRead(IndicationPin))
    {
      client.loop();
    }
  }

  client.loop();
}
