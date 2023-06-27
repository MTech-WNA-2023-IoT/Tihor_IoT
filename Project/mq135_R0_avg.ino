const unsigned long duration = 60000;

void setup() {
  Serial.begin(9600); // Baud rate
}

void loop() {
  float avg = 0.0; // Variable to store the average R0 value
  unsigned long startTime = millis();
  int counter = 0; // Counter for the number of iterations

  while (millis() - startTime < duration) {
    float sensorValue = 0; // Define variable for analog readings

    for (int x = 0; x < 500; x++) {
      sensorValue += analogRead(A0); // Add analog values of sensor 500 times
    }
    
    sensorValue /= 500.0; // Take average of readings

    float sensor_volt = sensorValue * (5.0 / 1023.0); // Convert average to voltage
    float RS_air = ((5.0 * 10.0) / sensor_volt) - 10.0; // Calculate RS in fresh air
    float R0 = RS_air / 4.4; // Calculate R0

    avg += R0; // Accumulate R0 values
    counter++; // Increment counter

    Serial.print("R0 = "); // Display "R0"
    Serial.println(R0); // Display value of R0
    delay(1000); // Wait 1 second
  }

  avg /= counter; // Calculate the average R0 value

  Serial.print("Average R0 = "); // Display "Average R0"
  Serial.println(avg); // Display the average R0 value
  delay(1000); // Wait 1 second
}
