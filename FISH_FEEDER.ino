#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

Servo myservo;

const char* ssid = "iot1";
const char* password = "iot123456";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

WiFiClient esp1234;
PubSubClient client(esp1234);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print(WiFi.status());
  Serial.println(WiFi.SSID());
  myservo.attach(4);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTTcallback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("iot62815")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  Serial.println("Before");
  client.subscribe("esp/feed");
  Serial.print("After");
}

void MQTTcallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "On") {
    // Servo control logic
    int i = 0;
    int second = 4;
    while (i < 4) {
      client.publish("esp/fst", "Feeding ");
      myservo.write(180);
      delay(2000);
      myservo.write(0);
      delay(2000);
      i++;
    }
    client.publish("esp/fst", "Finished");
  } else if (message == "Off") {
    myservo.write(0);
  }
}

void loop() {
  
  client.loop(); // Process MQTT client

}