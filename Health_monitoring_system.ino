#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "moto";  
const char* password = "12345678";
const char* server = "api.thingspeak.com";   
const String apiKey = "05C8N04AFXHQD0DN";
float temperature;

#define GY906_ADDRESS 0x5A
const int pulsePin = A0; // Analog pin connected to the HW827 pulse sensor

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  delay(100);
  Wire.begin(D1, D2); // Initialize I2C communication with GY-906 sensor
  Wire.setClock(400000); 

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);  // Pause for 2 seconds
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read the value from the pulse sensor
  int pulseValue = analogRead(pulsePin);
  int pulseValue1 = pulseValue / 10;
  
  // Read the temperature from the GY-906 sensor
  float temperature = readGY906Temperature();

  // Send data to ThingSpeak
  sendToThingSpeak(pulseValue1, temperature);

  // Display the pulse value and temperature on OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Health Monitoring");
  display.println("System");
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Pulse: ");
  display.println(pulseValue1);
  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print("Temperature: ");
  display.println(temperature);
  display.display();

  // Add a delay before reading the pulse value again
  delay(10000); // Adjust delay according to your application's requirements
}

float readGY906Temperature() {
  Wire.beginTransmission(GY906_ADDRESS);
  Wire.write(0x07); // Register address for ambient temperature data
  Wire.endTransmission(false); // Keep the connection active
  Wire.requestFrom(GY906_ADDRESS, 2); // Request 2 bytes of data
  
  // Combine the bytes to form the temperature value
  int16_t rawTemp = Wire.read() | (Wire.read() << 8);

  // Convert raw temperature data to Celsius
  float temperatureC = rawTemp * 0.02 + 37.15;
  
  return temperatureC;
}

void sendToThingSpeak(int value, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Create a WiFi client object

    HTTPClient http;

    // Construct the URL with the data to send
    String url = "https://api.thingspeak.com/update?api_key=";
    url += apiKey;
    url += "&field1=";
    url += String(value);
    url += "&field2=";
    url += String(temperature);

    Serial.print("Sending data to ThingSpeak: ");
    Serial.println(url);

    // Send HTTP GET request
    http.begin(client, server, 80, url); // Use the overloaded version of begin

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      
      Serial.println(response);
    } else {
      Serial.print("Error sending data to ThingSpeak. HTTP Response code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error: Not connected to WiFi.");
  }
}
