#include <Wire.h>

#define GY906_ADDRESS 0x5A // Default I2C address of GY-906 sensor
#define FEVER_THRESHOLD 100.4 // Threshold temperature for fever in Fahrenheit (37.5°C)

const int pulsePin = A0;  // Analog pin for pulse sensor
int pulseValue;            // Variable to store pulse sensor value
unsigned long previousMillis = 0;  // Variable to store previous time
unsigned long interval = 1000;     // Interval for calculating pulse rate (1 second)

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  Wire.begin(); // Initialize I2C communication
  Wire.setClock(400000); // Set I2C clock speed to 400kHz
  Wire.beginTransmission(GY906_ADDRESS); // Start communication with GY-906 sensor
  Wire.write(0x07); // Register address for ambient temperature data
  Wire.endTransmission(); // End transmission
}

void loop() {
  unsigned long currentMillis = millis();  // Get current time
  // Read the pulse sensor value
  pulseValue = analogRead(pulsePin);
  
  // Check if enough time has passed to calculate pulse rate
  if (currentMillis - previousMillis >= interval) {
    // Calculate pulse rate in beats per minute (BPM)
    float pulseRate = (pulseValue / 10.0); // Assuming pulse sensor outputs values from 0 to 1023
  
    // Print pulse rate to serial monitor
    Serial.print("Pulse Rate: ");
    Serial.print(pulseRate);
    Serial.println(" BPM");
    
    // Update previous time
    previousMillis = currentMillis;
  }
  
  // Read temperature from GY-906 sensor
  // Read 2 bytes of data
  Wire.requestFrom(GY906_ADDRESS, 2);
  
  // Combine the bytes to form the temperature value
  int16_t rawTemp = Wire.read() | (Wire.read() << 8);
  
  // Convert raw temperature data to Celsius
  float temperatureC = rawTemp * 0.02 - 273.15;
  
  // Convert Celsius to Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
  // Print temperature to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");
  
  // Check if temperature exceeds fever threshold
  if (temperatureF > FEVER_THRESHOLD) {
    Serial.println("Fever detected!");
    // Add additional actions here, such as activating an alarm or sending an alert
  } else {
    Serial.println("Fever NOT detected!");
  }
  
  delay(1000); // Read temperature and pulse rate every second
}
