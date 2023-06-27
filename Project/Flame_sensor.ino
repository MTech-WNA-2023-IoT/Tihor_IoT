const int flamePin = A0;       // Analog pin connected to the flame sensor output
const int calibrationReadings = 100;  // Number of readings to use for calibration
int flameThreshold = 0;  // Initial threshold value

void setup() {
  Serial.begin(9600);         // Initialize serial communication
  pinMode(flamePin, INPUT);   // Set the flamePin as an input
  calibrateSensor();  // Perform sensor calibration
}

void loop() {
  int flameValue = analogRead(flamePin);  // Read the analog value from the flame sensor

  // Print the flame sensor value
  Serial.print("Flame Sensor Value: ");
  Serial.println(flameValue);

  // Check if flame is detected
  if (flameValue < flameThreshold) {
    Serial.println("Flame Detected!");
    // Add your code here to trigger an action or send a notification
  }

  delay(1000);  // Delay between readings
}

void calibrateSensor() {
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
