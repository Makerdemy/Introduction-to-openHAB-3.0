#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"        

// Enter your Wi-Fi credentials here
#define wifi_ssid "abc"         
#define wifi_password "xyz"    

// Enter the host device IP address here
#define mqtt_server "192.168.0.122"  

// Defining MQTT channels for sensors
#define temperature_topic "esp32/dht/temperature"      
#define humidity_topic "esp32/dht/humidity"         
#define motion_topic "esp32/pir/motion"     

#define DHTPIN 4              
#define DHTTYPE DHT11          
#define PIRPIN 22

float temp;
float hum;
int pir;
    
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE); 

// Connects to the Wi-Fi
void setup_wifi() {
  delay(20);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
 Serial.println("");
 Serial.println("WiFi is connected ");
 Serial.print(WiFi.localIP());
 Serial.println("");
}

// Connects to the MQTT broker again if client is not connected
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP32Client")) {
      Serial.println("OK");
    } else {
      Serial.print("[Error] Not connected: ");
      Serial.print(client.state());
      Serial.println("Wait 5 seconds before retry.");
      delay(5000);
    }
  }
}

// Connecting to the openHAB MQTT server
void setup() {
  Serial.begin(115200);     
  setup_wifi();                              
  client.setServer(mqtt_server, 1883);    
  if (!client.connected()) {
    reconnect();
  }
}

// Sensor values are stored in the variables defined earlier and are published to the MQTT channels
void loop() {   
  
  bool isDetected = digitalRead(PIRPIN); 
    if(isDetected){
      pir=1;
    }
    else{
      pir=0;
    } 
    
    dht.begin(); 
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    
    // Publish values to MQTT topics
    client.publish(motion_topic, String(pir).c_str(), true);      
    Serial.println("Motion detection sent to MQTT.");
    delay(1000);
    
    client.publish(temperature_topic, String(temp).c_str(), true);   
    Serial.println("Temperature sent to MQTT.");
    delay(1000); 
    
    client.publish(humidity_topic, String(hum).c_str(), true);      
    Serial.println("Humidity sent to MQTT.");
    delay(1000);
    
    Serial.print("Motion : ");
    Serial.print(pir);
    Serial.print(" | Temperature : ");
    Serial.print(temp);
    Serial.print(" | Humidity : ");
    Serial.println(hum);

}
