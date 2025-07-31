# Air-Quality-Station
# 🌍 Air Quality Monitoring Station (Arduino + Blynk)

This project is a real-time indoor air quality monitor built with the **Arduino Uno WiFi Rev2** and **Grove sensor modules**. Sensor data is transmitted to a **Blynk IoT dashboard** where temperature, humidity, pressure, light, CO₂ levels, dust concentration, and general air quality are visualized.

---

## 📦 Features

- ✅ Real-time sensor readings on mobile and web (via Blynk)
- 🌡️ Temperature and Humidity (DHT22)
- 🫁 CO₂ concentration (Analog Grove Sensor)
- 🌫️ Dust concentration (GP2Y1010AU0F)
- 💨 Air Quality Index (Grove Air Quality v1.3)
- 💡 Ambient Light (TSL2561)
- ⬇️ Atmospheric Pressure (BMP280)
- 📉 Data smoothing and voltage clamping for realistic readings

---

## 🧰 Hardware Used

| Component                     | Description                                   |
|------------------------------|-----------------------------------------------|
| Arduino Uno WiFi Rev2        | Main microcontroller with built-in WiFi       |
| Grove DHT22                  | Digital Temp & Humidity Sensor (Pin D6)       |
| Grove CO₂ Sensor (MH-Z16)    | Analog CO₂, 0.4–2.0 V → 0–5000 ppm (A3)       |
| Grove Dust Sensor (GP2Y1010) | Analog Dust, smoothed output (Pin D8)         |
| Grove Air Quality v1.3       | Analog gas sensor, general VOC level (A0)     |
| Grove TSL2561                | Digital Light Sensor (I²C 0x29)               |
| Grove BMP280                 | Digital Pressure Sensor (I²C 0x77)            |
| Grove Base Shield            | For connecting sensors easily via Grove ports |

---

## 📊 Virtual Pin Assignments (Blynk)

| Virtual Pin | Sensor             | Units           | Typical Range     |
|-------------|--------------------|------------------|--------------------|
| V0          | Temperature (DHT22)| °C               | 20–35              |
| V1          | Humidity (DHT22)   | %                | 20–60              |
| V2          | CO₂ (Analog)       | ppm              | 400–2000 (indoor)  |
| V3          | Dust               | µg/m³            | 0–500              |
| V4          | Light (TSL2561)    | lux              | 0–40000            |
| V8          | Pressure (BMP280)  | hPa              | 980–1050           |
| V9          | Air Quality (volts)| V                | 0.1–3.0            |
| V10         | Air Quality Status | Label (text)     | Excellent–Hazardous |
| V11         | CO₂ Status         | Label (text)     | Good–Unhealthy     |

---

## 📱 Blynk Dashboard

- Uses **Simple Chart widgets** connected to V0–V4, V8–V9
- **Label widgets** for air quality and CO₂ status (V10, V11)
- Update rate: ~5 seconds for fast response
- Automatically reconnects to WiFi and Blynk cloud

---

## ⚙️ Setup Instructions

1. **Clone the Code** into Arduino IDE  
2. **Install Required Libraries**:
   - Blynk (via Library Manager)
   - WiFiNINA
   - Adafruit BMP280
   - Adafruit Unified Sensor
   - Adafruit TSL2561
   - DHT sensor library (Adafruit)

3. **Connect Sensors** to Grove Base Shield:
   - DHT22 to D6
   - CO₂ to A3
   - Air Quality to A0
   - Dust Sensor to D8
   - TSL2561 and BMP280 on I²C

4. **Enter Your WiFi + Blynk Credentials** in the code

5. **Upload Code to Arduino Uno WiFi Rev2**

6. **Open Serial Monitor @ 9600 baud**  
   You should see all sensor outputs printed every few seconds.

---

## 🧠 Calibration & Notes

- **CO₂ sensor**: Analog output (0.4–2.0 V). Values above this are clamped. May require outdoor zeroing.
- **Dust sensor**: Uses `pulseIn()` and exponential smoothing. Readings stabilize after ~30s.
- **Air Quality sensor**: Outputs a general voltage trend for VOCs — not specific ppm.
- **Light sensor**: Accurate from ~0–40,000 lux. Calibrate in ambient vs covered light.
- **Pressure**: Standard sea-level range ~1013 hPa.

---

## 🧪 To Do

- [ ] Add local OLED screen or buzzer alert
- [ ] Log data to SD card or cloud (Google Sheets / InfluxDB)
- [ ] Add support for digital CO₂ (MH-Z19B) via UART

---

## 📸 Screenshots

> _Add screenshots of your Blynk mobile dashboard and real sensor output here!_

---

## 📄 License

MIT License. Use freely for educational and personal projects.

---

## 🙌 Credits

Built with Arduino, Grove sensors, and Blynk IoT platform  
Created by [Your Name or Team]
