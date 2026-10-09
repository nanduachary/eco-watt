#include <DHT.h>
#include <NewPing.h>

// --- Pin Definitions ---
#define PIR_PIN       13
#define LDR_PIN       14
#define DHTPIN        4
#define TRIG_PIN      5
#define ECHO_PIN      18
#define VOLTAGE_PIN   32
#define CURRENT_PIN   35

// Updated Relay Assignments
#define RELAY_LIGHT_1 25 // IN1
#define RELAY_LIGHT_2 26 // IN2
#define RELAY_LIGHT_3 27 // IN3
#define RELAY_FAN     33 // IN4

// --- Sensor Setup ---
DHT dht(DHTPIN, DHT11);
NewPing sonar(TRIG_PIN, ECHO_PIN, 400);

// --- System Variables ---
unsigned long lastMotionTime = 0;
const unsigned long gracePeriod = 15000; // 15 seconds for testing
const float targetTemp = 26.0;           // Celsius threshold 

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(PIR_PIN, INPUT_PULLDOWN);
  pinMode(LDR_PIN, INPUT);
  
  pinMode(RELAY_LIGHT_1, OUTPUT);
  pinMode(RELAY_LIGHT_2, OUTPUT);
  pinMode(RELAY_LIGHT_3, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  
  // Relays are Active-LOW (HIGH = OFF)
  digitalWrite(RELAY_LIGHT_1, HIGH);
  digitalWrite(RELAY_LIGHT_2, HIGH);
  digitalWrite(RELAY_LIGHT_3, HIGH);
  digitalWrite(RELAY_FAN, HIGH);
  
  Serial.println("Smart Campus System Initializing...");
  delay(2000); 
}

void loop() {
  // 1. Read Sensors
  int pirState = digitalRead(PIR_PIN);
  int distance = sonar.ping_cm();
  int ldrState = digitalRead(LDR_PIN); 
  float temp = dht.readTemperature();
  
  // Read Energy 
  int rawVolts = analogRead(VOLTAGE_PIN);
  int rawCurrent = analogRead(CURRENT_PIN);
  float voltage = ((rawVolts * 3.3) / 4095.0) * 5.0; 
  float current = abs((((rawCurrent * 3.3) / 4095.0) - 1.65) / 0.100); 
  float powerWatts = voltage * current;
  
  // 2. Occupancy & Grace Period Logic
  if (pirState == HIGH || (distance > 0 && distance < 100)) {
    lastMotionTime = millis(); 
  }
  
  bool isOccupied = (millis() - lastMotionTime) < gracePeriod;
  
  // 3. Smart Control Logic
  if (isOccupied) {
    
    // Daylight Harvesting Logic (Controls IN1, IN2, IN3)
    if (ldrState == HIGH) { 
      digitalWrite(RELAY_LIGHT_1, LOW); // Turn Light 1 ON
      digitalWrite(RELAY_LIGHT_2, LOW); // Turn Light 2 ON
      digitalWrite(RELAY_LIGHT_3, LOW); // Turn Light 3 ON
    } else {
      digitalWrite(RELAY_LIGHT_1, HIGH); // Turn Light 1 OFF
      digitalWrite(RELAY_LIGHT_2, HIGH); // Turn Light 2 OFF
      digitalWrite(RELAY_LIGHT_3, HIGH); // Turn Light 3 OFF
    }
    
    // Climate Control Logic (Controls IN4)
    if (!isnan(temp) && temp >= targetTemp) {
      digitalWrite(RELAY_FAN, LOW);   // Turn Fan ON
    } else {
      digitalWrite(RELAY_FAN, HIGH);  // Turn Fan OFF
    }
    
  } else {
    // Turn all appliances OFF if room is empty
    digitalWrite(RELAY_LIGHT_1, HIGH);
    digitalWrite(RELAY_LIGHT_2, HIGH);
    digitalWrite(RELAY_LIGHT_3, HIGH);
    digitalWrite(RELAY_FAN, HIGH); 
  }
  
  // 4. DETAILED TELEMETRY OUTPUT
  Serial.println("=========================================");
  Serial.print("Status:       "); Serial.println(isOccupied ? "ROOM OCCUPIED" : "EMPTY (Power Saving)");
  Serial.print("PIR Sensor:   "); Serial.println(pirState == HIGH ? "Motion Detected (1)" : "Still (0)");
  Serial.print("Ultrasonic:   "); Serial.print(distance); Serial.println(" cm");
  Serial.print("Temperature:  "); Serial.print(temp); Serial.println(" °C");
  Serial.print("LDR (Light):  "); Serial.println(ldrState == LOW ? "Bright (0)" : "Dark (1)");
  
  Serial.println("--- Power Data ---");
  Serial.print("Voltage:      "); Serial.print(voltage); Serial.println(" V");
  Serial.print("Current:      "); Serial.print(current); Serial.println(" A");
  Serial.print("Power Usage:  "); Serial.print(powerWatts); Serial.println(" W");
  
  Serial.println("--- Relay States ---");
  Serial.print("IN1 (Light 1): "); Serial.println(digitalRead(RELAY_LIGHT_1) == LOW ? "ON" : "OFF");
  Serial.print("IN2 (Light 2): "); Serial.println(digitalRead(RELAY_LIGHT_2) == LOW ? "ON" : "OFF");
  Serial.print("IN3 (Light 3): "); Serial.println(digitalRead(RELAY_LIGHT_3) == LOW ? "ON" : "OFF");
  Serial.print("IN4 (Fan):     "); Serial.println(digitalRead(RELAY_FAN) == LOW ? "ON" : "OFF");
  Serial.println("=========================================\n");
  
  delay(2000); 
}