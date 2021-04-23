#include <WiFi.h>
#include <PubSubClient.h>

// Enter your Wi-Fi credentials here
#define ssid "abc"
#define password "xyz"

// Enter the host device IP address here
#define mqtt_server "192.168.0.122"

#define buzzerPin 4
#define relayPin 2

WiFiClient espClient;
PubSubClient client(espClient); 

void setup() {
  Serial.begin(115200);  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // Setting the relay and buzzer pins as output pins
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}


// Connects to the Wi-Fi
void setup_wifi() {
  delay(10);
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

// The callback function extracts the string from MQTT channels
long lastMsg = 0;
char msg[50];
int value = 0;

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Check for the relay channel message and set the relay pin to HIGH/LOW accordingly
  if (String(topic) == "esp32/relay") {
    Serial.print("Changing relay output to ");
    if(messageTemp == "ON"){
      Serial.println(messageTemp);
      digitalWrite(relayPin, HIGH);
    }
    else if(messageTemp == "OFF"){
      Serial.println(messageTemp);
      digitalWrite(relayPin, LOW);
    }
  }

  // Check for the buzzer channel message and set the relay pin to HIGH/LOW accordingly
  if (String(topic) == "esp32/buzzer") {
    Serial.print("Changing buzzer output to ");
    if(messageTemp == "ON"){
      Serial.println(messageTemp);
      digitalWrite(buzzerPin, HIGH);
    }
    else if(messageTemp == "OFF"){
      Serial.println(messageTemp);
      digitalWrite(buzzerPin, LOW);
    }
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      
      // Subscribe to the relay and buzzer MQTT channels
      client.subscribe("esp32/relay");
      client.subscribe("esp32/buzzer");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
