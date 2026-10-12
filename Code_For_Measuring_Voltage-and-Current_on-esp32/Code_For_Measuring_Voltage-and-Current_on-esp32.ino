#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Wi-Fi Credentials
const char* ssid = "FoxHome";
const char* password = "ceco0804$";

// Initialize LCD (Most I2C backpacks use address 0x27 or 0x3F, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Voltage Sensing 
const int voltagePin = 32; 
const float R1 = 10000.0;  // 10 kOhm
const float R2 = 20000.0;  // 20 kOhm

// Current Sensing (GPIO36 belongs to ADC1, safe to use with Wi-Fi)
const int currentPin = 33;      
const float MV_PER_AMP = 100.0;   
const int SAMPLES = 100;  

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize LCD and turn on backlight
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Set pin modes
  pinMode(voltagePin, INPUT);
  pinMode(currentPin, INPUT);

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
  // --- VOLTAGE MEASUREMENT ---
  float pinMilliVolts = analogReadMilliVolts(voltagePin);
  float voltage_pin = pinMilliVolts / 1000.0;
  float real_voltage = voltage_pin * ((R1 + R2) / R2);

  // --- CURRENT MEASUREMENT ---
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogReadMilliVolts(currentPin);
    delay(1);
  }
  float avgMilliVolts = sum / (float)SAMPLES;
  
  
  float zeroOffset = 2500.0; 
  float currentAmps = (avgMilliVolts - zeroOffset) / MV_PER_AMP;
 
  // --- DISPLAY ON SERIAL MONITOR ---
  Serial.print("Voltage: ");
  Serial.print(real_voltage, 2);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(currentAmps, 2);
  Serial.println(" A");

  // --- DISPLAY ON 16x2 LCD ---
  // Row 1: Voltage (Trailing spaces clear ghost characters from old readings)
  lcd.setCursor(0, 0);
  lcd.print("Volt: ");
  lcd.print(real_voltage, 2);
  lcd.print(" V      "); 

  // Row 2: Current
  lcd.setCursor(0, 1);
  lcd.print("Amps: ");
  lcd.print(currentAmps, 2);
  lcd.print(" A      ");
  
  delay(500); // Refresh screen data every half second
}
