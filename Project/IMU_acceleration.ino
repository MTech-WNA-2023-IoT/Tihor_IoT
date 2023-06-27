#include <Arduino_LSM6DS3.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "BSNL FTTH";
const char* password = "ihatemyself";

// MQTT broker configuration
const char* mqttBroker = "35.244.35.92";
const int mqttPort = 1883;
const char* mqttClientId = "nano33";
const char* mqttTopic = "sensor/data";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

float prevX = 0.0, prevY = 0.0, prevZ = 0.0, prevA = 0.0, prevB = 0.0, prevC = 0.0;
const float threshold = 0.75; // Adjust the threshold value as needed

float aX, aY, aZ;
float gX, gY, gZ;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (true); // halt program
  } 
  Serial.println("IMU initialized!");
  connectToWiFi();
  connectToMQTT();
}

void loop() {

  const char * spacer = ", ";

  if (
    IMU.accelerationAvailable() 
    && IMU.gyroscopeAvailable()
  ) {      
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);
    // Serial.print(aX); Serial.print(spacer);
    // Serial.print(aY); Serial.print(spacer);
    // Serial.print(aZ); Serial.print(spacer);
    // Serial.print(gX); Serial.print(spacer);
    // Serial.print(gY); Serial.print(spacer);
    // Serial.println(gZ);
    delay(1000);
  }

    // Check for changes in acceleration values
  if (abs(aX - prevX) > threshold || 
      abs(aY - prevY) > threshold || 
      abs(aZ - prevZ) > threshold ||
      abs(gX - prevA) > threshold || 
      abs(gY - prevB) > threshold || 
      abs(gZ - prevC) > threshold) {
        Serial.println("Vibration detected!");
        Serial.println("---------------------------------------------");
        Serial.println("aX: ");
        Serial.print(aX);
        Serial.print("  aY: ");
        Serial.print(aY);
        Serial.print("  aZ: ");
        Serial.println(aZ);
        Serial.println("---------------------------------------------");
        Serial.print("gX: ");
        Serial.print(gX);
        Serial.print("  gY: ");
        Serial.print(gY);
        Serial.print("  gZ: ");
        Serial.println(gZ);
         //Read sensor data
        String sensorData = readSensorData();

        // Publish data to MQTT topic
        mqttClient.publish(mqttTopic, String(sensorData).c_str());
        delay(1000); // Adjust delay as needed
  }

  // Update previous values
  prevX = aX;
  prevY = aY;
  prevZ = aZ;
  prevA = gX;
  prevB = gY;
  prevC = gZ;

  delay(100); // Adjust delay as needed

  // if (!mqttClient.connected()){
  //   reconnectMQTT();
  // }
  // mqttClient.loop();

}

String readSensorData() {
  char data[50]; // Create a temporary char array to hold the concatenated string
  
  // Concatenate the float values with '#' delimiter
  snprintf(data, sizeof(data), "#%.2f#%.2f#%.2f#%.2f#%.2f#%.2f", aX, aY, aZ, gX, gY, gZ);
  
  return String(data); // Convert the char array to a String object and return it
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(1000);
  }
  Serial.println("Connected to Wi-Fi!");
}

void connectToMQTT() {
  Serial.print("Connecting to MQTT broker...");
  mqttClient.setServer(mqttBroker, mqttPort);
  while (!mqttClient.connected()) {
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed to connect to MQTT broker. Retrying...");
      delay(1000);
    }
  }
}
