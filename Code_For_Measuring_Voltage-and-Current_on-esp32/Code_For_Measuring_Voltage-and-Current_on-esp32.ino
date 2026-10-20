#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_INA219.h>

// Wi-Fi Credentials
const char* ssid = "Name-Wifi";
const char* password = "Password-wifi";

// Initialize LCD (Most I2C backpacks use address 0x27 or 0x3F, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize INA219 Current Sensor (I2C address default 0x40, uses SDA/SCL pins)
Adafruit_INA219 ina219;

// Voltage Sensing (External Divider)
const int voltagePin = 32; 
const float R1 = 10000.0;  // 10 kOhm
const float R2 = 1500.0;   // 1.5 kOhm

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize LCD and turn on backlight
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Initialize INA219 sensor over I2C (SDA/SCL pins)
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("INA219 Error!");
    while (1) { delay(10); }
  }

  // Set pin modes
  pinMode(voltagePin, INPUT);

  // Connect to Wi-Fi Network
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to Wi-Fi");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connecting");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(250);
  }
    
  // If connection is successful 
  Serial.println("\nConnected to the Wi-Fi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Show local IP on the LCD for 2 seconds
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP().toString());
  delay(2000); 
  
  lcd.clear();
}

void loop() {
  // --- VOLTAGE MEASUREMENT (External Divider) ---
  float pinMilliVolts = analogReadMilliVolts(voltagePin);
  float voltage_pin = pinMilliVolts / 1000.0;
  float real_voltage = voltage_pin * ((R1 + R2) / R2);

  // --- CURRENT MEASUREMENT (INA219 in mA) ---
  float current_mA = ina219.getCurrent_mA();

  // --- DISPLAY ON SERIAL MONITOR ---
  Serial.print("Voltage: ");
  Serial.print(real_voltage, 2);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current_mA, 1);
  Serial.println(" mA");

  // --- DISPLAY ON 16x2 LCD ---
  // Row 1: Voltage
  lcd.setCursor(0, 0);
  lcd.print("Volt: ");
  lcd.print(real_voltage, 2);
  lcd.print(" V      "); 

  // Row 2: Current in mA
  lcd.setCursor(0, 1);
  lcd.print("Curr: ");
  lcd.print(current_mA, 1);
  lcd.print(" mA     ");
  
  delay(500); // Refresh screen data every half second
}
