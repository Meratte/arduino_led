#include <WiFiNINA.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the pin for the DHT sensor
#define DHTPIN 2
#define DHTTYPE DHT11 // Change to DHT22 if you are using the DHT22 sensor
#define LED_PIN 13

DHT dht(DHTPIN, DHTTYPE);

float temperature;
float humidity;
bool ledState;

const char SSID[] = "your_network_ssid";
const char PASS[] = "your_network_password";

// WiFi connection handler
WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// Initialize the LCD with the I2C address 0x27 (this might be different for your specific LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void initProperties() {
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(humidity, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(ledState, READWRITE, ON_CHANGE, onLedStateChange);
}

void onLedStateChange() {
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);
  dht.begin();

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  // Initialize IoT Cloud properties
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PASS);
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi");

  // Debugging message
  Serial.println("Setup complete");
  lcd.clear();
  lcd.print("Setup complete");
}

void loop() {
  ArduinoCloud.update();
  
  float newHumidity = dht.readHumidity();
  float newTemperature = dht.readTemperature();

  if (!isnan(newHumidity) && !isnan(newTemperature)) {
    humidity = newHumidity;
    temperature = newTemperature;

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Print data to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print(" %");

  delay(2000);
}

/*
  Since Humidity is READ_WRITE variable, onHumidityChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onHumidityChange()  {
  // Add your code here to act upon Humidity change
}
/*
  Since Temperature is READ_WRITE variable, onTemperatureChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTemperatureChange()  {
  // Add your code here to act upon Temperature change
}