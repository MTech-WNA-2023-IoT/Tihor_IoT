const int gasPin = A0;         // Analog pin connected to the sensor output
const int threshold = 1000;    // Threshold gas concentration level
const int LPGthresh = 1000;
const int NGthresh = 1000;
const int COthresh = 50;
float R0 = 14.00;
float m = -0.318; //Slope 
float b = 1.133; //Y-Intercept

void setup() {
  Serial.begin(9600);         // Initialize serial communication
  pinMode(gasPin, INPUT);     // Set the gasPin as an input
}

void loop() {
  int sensorValue = analogRead(gasPin);     // Read the analog value from the sensor
  // double ppm = CO_ppm(sensorValue);
  // Serial.print("% Sensor Value: ");
  // Serial.println(ppm);
  // Serial.print(sensorValue);

  float sensor_volt; //Define variable for sensor voltage 
  float RS_gas; //Define variable for sensor resistance  
  float ratio; //Define variable for ratio
  // float sensorValue = analogRead(gas_sensor); //Read analog values of sensor  
  sensor_volt = sensorValue*(5.0/1023.0); //Convert analog values to voltage 
  RS_gas = ((5.0*10.0)/sensor_volt)-10.0; //Get value of RS in a gas
  ratio = RS_gas/R0;  // Get ratio RS_gas/RS_air
 
  double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  double percentage = ppm/10000; //Convert to percentage 
  Serial.print("Sensor Value: ");
  Serial.println(ppm);
  
  Serial.print("percentage: ");
  Serial.println(percentage);
  Serial.println("--------------");

  if (ppm > COthresh) {
    Serial.println("------------------------------------------");
    Serial.println("CO2 concentration exceeds threshold!");
    Serial.println("------------------------------------------");
    // Add your code here to trigger an alert or take specific actions
  }

  
  // Wait for a few seconds before taking the next reading
  delay(500);
}


// int LPG_ppm(double rawValue){

//     double ppm = 26.572*exp(1.2894*(rawValue*3.3/4095)); //Multiply raw analog value by 3.3/4095 to convert to a voltage
//     return ppm;
// }

// //Calculate NG PPM
// int NG_ppm(double rawValue){

//     double ppm = 10.938*exp(1.7742*(rawValue*3.3/4095)); 
//     return ppm;
// }

//Calculate CO PPM
// int CO_ppm(double rawValue){

//     double ppm = 3.027*exp(1.0698*(rawValue*3.3/4095));
//     return ppm;
// }

