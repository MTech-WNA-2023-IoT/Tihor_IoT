#include <Arduino_LSM6DS3.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//------------------------------ sensors ---------------------------------------//

// Flame sensor
const int flamePin = A1;       // Analog pin connected to the flame sensor output
const int calibrationReadings = 100;  // Number of readings to use for calibration
int flameThreshold = 0;  // Initial threshold value
int flameValue;

// Gas sensor setup
const int gasPin = A0;         // Analog pin connected to the sensor output
const int COthresh = 50;
float R0 = 10.00;
float m = -0.318; //Slope 
float b = 1.133; //Y-Intercept
double ppm;

// Accelerometer and gyrometer setup 
float prevX = 0.0, prevY = 0.0, prevZ = 0.0, prevA = 0.0, prevB = 0.0, prevC = 0.0;
const float threshold = 0.75; // Adjust the threshold value as needed
float aX, aY, aZ;
float gX, gY, gZ;

//----------------------------------- Wifi / mqtt broker ---------------------//

// Wi-Fi credentials
const char* ssid = "BSNL FTTH";
const char* password = "ihatemyself";

// MQTT broker configuration
const char* mqttBroker = "34.100.247.89";
const int mqttPort = 1883;
const char* mqttClientId = "nano33";
const char* mqttTopic = "sensor/data";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//------------------------ Functions --------------------------------------------//

void connectToWiFi(){
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(1000);
  }
  Serial.println("Connected to Wi-Fi!");
}

void connectToMQTT(){
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

void calibrateSensor(){
  Serial.println("-----------------------------Calibrating Sensor...-------------------------------------------------");
  int sensorReading = 0;

  // Take multiple readings to calculate the average value
  for (int i = 0; i < calibrationReadings; i++) {
    sensorReading += analogRead(flamePin);
    delay(10);
  }

  // Calculate the average value as the threshold
  flameThreshold = sensorReading / calibrationReadings;

  Serial.print("Calibrated Threshold: ");
  Serial.println(flameThreshold);
  Serial.println("Calibration Complete!");
  delay(5000);

}

void axl_gyro(){
  const char * spacer = ", ";

  if (
    IMU.accelerationAvailable() 
    && IMU.gyroscopeAvailable()
  ) {      
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);
    Serial.println("------------------------------------------");
    Serial.print(aX); Serial.print(spacer);
    Serial.print(aY); Serial.print(spacer);
    Serial.print(aZ); Serial.print(spacer);
    Serial.print(gX); Serial.print(spacer);
    Serial.print(gY); Serial.print(spacer);
    Serial.println(gZ);
    delay(1000);
  }

  //   // Check for changes in acceleration values
  // if (abs(aX - prevX) > threshold || 
  //     abs(aY - prevY) > threshold || 
  //     abs(aZ - prevZ) > threshold ||
  //     abs(gX - prevA) > threshold || 
  //     abs(gY - prevB) > threshold || 
  //     abs(gZ - prevC) > threshold) {
  //       Serial.println("Vibration detected!");
  //       Serial.println("---------------------------------------------");
  //       Serial.println("aX: ");
  //       Serial.print(aX);
  //       Serial.print("  aY: ");
  //       Serial.print(aY);
  //       Serial.print("  aZ: ");
  //       Serial.println(aZ);
  //       Serial.println("---------------------------------------------");
  //       Serial.print("gX: ");
  //       Serial.print(gX);
  //       Serial.print("  gY: ");
  //       Serial.print(gY);
  //       Serial.print("  gZ: ");
  //       Serial.println(gZ);
  //       delay(1000); // Adjust delay as needed
  // }
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

double MQ135(){
  const int gasPin = A0;         // Analog pin connected to the sensor output
  int sensorValue = analogRead(gasPin);     // Read the analog value from the sensor
  float sensor_volt; //Define variable for sensor voltage 
  float RS_gas; //Define variable for sensor resistance  
  float ratio; //Define variable for ratio

  sensor_volt = sensorValue*(5.0/1023.0); //Convert analog values to voltage 
  RS_gas = ((5.0*10.0)/sensor_volt)-10.0; //Get value of RS in a gas
  ratio = RS_gas/R0;  // Get ratio RS_gas/RS_air

  double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  double percentage = ppm/10000; //Convert to percentage 
  Serial.print("Gas ppm: ");
  Serial.println(ppm);
  
  // Serial.print("percentage: ");
  // Serial.println(percentage);
  // Serial.println("--------------");

  // if (ppm > COthresh) {
  //   Serial.println("------------------------------------------");
  //   Serial.println("CO2 concentration exceeds threshold!");
  //   Serial.println("------------------------------------------");
  //   // Add your code here to trigger an alert or take specific actions
  // }
  return ppm;
}

int flame_sensor(){
  flameValue = analogRead(flamePin);  // Read the analog value from the flame sensor
// Print the flame sensor value
  Serial.print("Flame Sensor: ");
  Serial.println(flameValue);
  Serial.println();

  // // Check if flame is detected
  // if (flameValue < flameThreshold) {
  //   Serial.println("Flame Detected!");
  //   // Add your code here to trigger an action or send a notification
  // }
  return flameValue;
}

String readSensorData(float ax, float ay, float az, float gx, float gy, float gz, double ppm, int flameValue){
  // Create a JSON document
  StaticJsonDocument<200> doc;

  // Populate the JSON document with sensor data
  doc["acceleration"]["x"] = ax;
  doc["acceleration"]["y"] = ay;
  doc["acceleration"]["z"] = az;
  doc["gyroscope"]["x"] = gx;
  doc["gyroscope"]["y"] = gy;
  doc["gyroscope"]["z"] = gz;
  doc["ppm"] = ppm;
  doc["flameValue"] = flameValue;

  // Serialize the JSON document to a string
  String jsonData;
  serializeJson(doc, jsonData);

  return jsonData;
} 

//------------------------------- setup -----------------------------------//
void setup(){
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (true); // halt program
  } 
  Serial.println("IMU initialized!");

  pinMode(gasPin, INPUT);     // Set the gasPin as an input
  pinMode(flamePin, INPUT);   // Set the flamePin as an input
  calibrateSensor();  // Perform sensor calibration
  connectToWiFi();
  connectToMQTT();

}

//---------------------------------- main -------------------------------------//
void loop() {
  // Collect sensor data
  axl_gyro();
  double ppm = MQ135();
  int flame = flame_sensor();

  // Check if any of the values cross the threshold
  if (ppm > COthresh || flame < flameThreshold ||
      abs(aX - prevX) > threshold ||
      abs(aY - prevY) > threshold ||
      abs(aZ - prevZ) > threshold ||
      abs(gX - prevA) > threshold ||
      abs(gY - prevB) > threshold ||
      abs(gZ - prevC) > threshold) {

    // Print relevant information
    if (ppm > COthresh) {
      Serial.println("------------------------------------------");
      Serial.println("CO2 concentration exceeds threshold!");
      Serial.println("------------------------------------------");
    }
    if (flame < flameThreshold) {
      Serial.println("------------------------------------------");
      Serial.println("Flame Detected!");
      Serial.println("------------------------------------------");
    }
    if (abs(aX ) > threshold || abs(aY) > threshold || abs(aZ) > threshold || abs(gX) > threshold || abs(gY) > threshold || abs(gZ) > threshold) {
      Serial.println("--------------------------------------------");
      Serial.println("Vibration detected!");
      Serial.println("aX: ");
      Serial.print(aX);
      Serial.print("  aY: ");
      Serial.print(aY);
      Serial.print("  aZ: ");
      Serial.println(aZ);
      Serial.print("gX: ");
      Serial.print(gX);
      Serial.print("  gY: ");
      Serial.print(gY);
      Serial.print("  gZ: ");
      Serial.println(gZ);

    }

    // Convert the sensor data into JSON format
    String sensorData = readSensorData(aX, aY, aZ, gX, gY, gZ, ppm, flameValue);

    // Publish data to MQTT topic
    mqttClient.publish(mqttTopic, sensorData.c_str());
  }

  delay(3000); // Adjust delay as needed
}

