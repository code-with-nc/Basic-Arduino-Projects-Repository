#include <Wire.h>

#define MAX30100_ADDRESS 0x57 // I2C address of MAX30100
#define LED_PIN D6 // LED pin
#define MAX30100_SCL D7 // SCL pin for MAX30100
#define MAX30100_SDA D8 // SDA pin for MAX30100

void setup() {
  Serial.begin(9600);
  Wire.begin(MAX30100_SDA, MAX30100_SCL); // Initialize I2C communication with MAX30100
  Wire.setClock(400000); // Set I2C clock speed to 400kHz
  
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  
  // Initialize MAX30100
  writeRegister(MAX30100_ADDRESS, 0x08, 0x03); // Configuration: SpO2 mode, sample rate = 100Hz
  writeRegister(MAX30100_ADDRESS, 0x09, 0x03); // LED pulse width = 1600us (maximum)
  writeRegister(MAX30100_ADDRESS, 0x02, 0x07); // Enable interrupts
  
  delay(1000); // Wait for sensor to stabilize
}

void loop() {
  // Read the pulse value
  int pulseValue = readPulseValue();

  // Print the pulse value
  Serial.print("Pulse Value: ");
  Serial.println(pulseValue);

  // Turn LED on if pulse detected
  if (pulseValue > 0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(1000); // Adjust delay according to your application's requirements
}

// Function to write data to a register of the MAX30100 sensor
void writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Function to read the pulse value from the MAX30100 sensor
int readPulseValue() {
  uint8_t data[6];
  
  // Read 6 bytes of data from the sensor
  Wire.beginTransmission(MAX30100_ADDRESS);
  Wire.write(0x00); // FIFO Data Register
  Wire.endTransmission(false);
  Wire.requestFrom(MAX30100_ADDRESS, 6);

  // Combine the bytes to form the pulse value
  if (Wire.available() == 6) {
    for (int i = 0; i < 6; i++) {
      data[i] = Wire.read();
    }
    // Calculate pulse value from the data
    int pulseValue = (data[0] << 8) | data[1];
    return pulseValue;
  } else {
    return 0;
  }
}
