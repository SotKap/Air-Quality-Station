#define BLYNK_TEMPLATE_ID "TMPL4m4SF8C-z"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "x1otM4GbKU4jEva7UzY9-ZCtK50KaksO"

#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_BMP280.h>

char ssid[] = "Palladio_Students_Primary";
char pass[] = "L0R5a6Qlbm";

// ----- Sensor Pins -----
#define DHTPIN 6
#define DHTTYPE DHT22
#define CO2_ANALOG_PIN A3
#define DUST_PIN 8
#define AIR_QUALITY_PIN A0

// ----- Sensor Objects -----
DHT dht(DHTPIN, DHTTYPE);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(0x29, 12345);
Adafruit_BMP280 bmp;

BlynkTimer timer;
float prevCO2 = 0;  // for smoothing
// ----- Dust Sensor Variables -----
unsigned long lowPulseOccupancy = 0;
unsigned long sampleStart = 0;
const unsigned long sampleTime = 30000UL;  // 30 seconds
float prevConcentration = 0;

// ----- Blynk Debug -----
BLYNK_CONNECTED() {
  Serial.println("✅ Blynk Connected.");
}

BLYNK_DISCONNECTED() {
  Serial.println("❌ Blynk Disconnected!");
}

void sendDHTandCO2() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) {
    Blynk.virtualWrite(V0, t);
    delay(100);
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" °C  ");
  }

  if (!isnan(h)) {
    Blynk.virtualWrite(V1, h);
    delay(100);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %  ");
  }

float voltage = analogRead(CO2_ANALOG_PIN) * (5.0 / 1023.0);

// Clamp to sensor range
if (voltage < 0.4) voltage = 0.4;
if (voltage > 2.0) voltage = 2.0;

float co2ppm = (voltage - 0.4) * (5000.0 / (4.0 - 0.4));  // Scale: 0.4V–2.0V → 0–5000 ppm
co2ppm = constrain(co2ppm, 0, 5000);  // Final clamp

// Smoothing
co2ppm = 0.7 * prevCO2 + 0.3 * co2ppm;
prevCO2 = co2ppm;

int analogValue = analogRead(CO2_ANALOG_PIN);
Serial.print("Raw CO2 Analog: ");
Serial.println(analogValue);

Blynk.virtualWrite(V2, co2ppm);

Serial.print("CO2 Voltage: ");
Serial.print(voltage, 2);
Serial.print(" V  CO2: ");
Serial.print(co2ppm);
Serial.println(" ppm");
}

void sendLight() {
  sensors_event_t event;
  tsl.getEvent(&event);

  if (event.light && event.light < 65535.0) {
    Blynk.virtualWrite(V4, event.light);
    delay(100);
    Serial.print("Light: ");
    Serial.print(event.light);
    Serial.println(" lux");
  } else {
    Serial.println("Light: Overrange or error");
  }
}

void sendPressure() {
  float pressure_hPa = bmp.readPressure() / 100.0F;
  Blynk.virtualWrite(V8, pressure_hPa);
  delay(100);
  Serial.print("Pressure: ");
  Serial.print(pressure_hPa);
  Serial.println(" hPa");
}

void sendAirQuality() {
  int sensorValue = analogRead(AIR_QUALITY_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  Blynk.virtualWrite(V9, voltage);
  delay(100);

  String status;
  if (voltage < 1.0) {
    status = "Excellent";
  } else if (voltage < 1.5) {
    status = "Good";
  } else if (voltage < 2.0) {
    status = "Moderate";
  } else if (voltage < 3.0) {
    status = "Poor";
  } else {
    status = "Hazardous";
  }

  Blynk.virtualWrite(V10, status);
  Serial.print("Air Quality Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V - Status: ");
  Serial.println(status);
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  pinMode(DUST_PIN, INPUT);

  if (!tsl.begin()) {
    Serial.println("TSL2561 not detected.");
  } else {
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
    Serial.println("TSL2561 ready.");
  }

  if (!bmp.begin(0x77)) {
    Serial.println("Could not find BMP280 at 0x77");
  } else {
    Serial.println("BMP280 ready.");
  }

  sampleStart = millis();

  // ✅ Staggered timers to reduce load
  timer.setInterval(10000L, sendDHTandCO2);    // every 10s
  timer.setInterval(15000L, sendLight);        // every 15s
  timer.setInterval(20000L, sendPressure);     // every 20s
  timer.setInterval(25000L, sendAirQuality);   // every 25s
}

void loop() {
  // ✅ Reconnect if WiFi lost
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi...");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  }

  Blynk.run();
  timer.run();

  // Dust logic
  unsigned long duration = pulseIn(DUST_PIN, LOW, 1000);
  if (duration > 0 && duration < 10000) {
    lowPulseOccupancy += duration;
  }

  if (millis() - sampleStart >= sampleTime) {
    float ratio = lowPulseOccupancy / (sampleTime * 10.0);
    ratio = constrain(ratio, 0.0, 1.0);

    float concentration = 1.1 * pow(ratio, 3)
                        - 3.8 * pow(ratio, 2)
                        + 520 * ratio + 0.62;
    concentration = constrain(concentration, 0, 1000.0);
    concentration = 0.7 * prevConcentration + 0.3 * concentration;
    prevConcentration = concentration;

    Blynk.virtualWrite(V3, concentration);
    delay(100);

    Serial.print("Ratio: ");
    Serial.print(ratio, 4);
    Serial.print("  Dust (smoothed): ");
    Serial.print(concentration);
    Serial.println(" µg/m³");

    lowPulseOccupancy = 0;
    sampleStart = millis();
  }
}
