/**
 * ESP32 Arduino Sketch - Send Sensor Data to Smart Nose API
 * 
 * Sketch ini mengirim data sensor ke Smart Nose TPA via Vercel API
 * Update `VERCEL_URL` dan WiFi credentials sesuai dengan setup Anda
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ===== WiFi Configuration =====
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ===== API Configuration =====
// Update URL ini dengan domain Vercel Anda
const char* vercelUrl = "https://your-project.vercel.app/api/readings";

// Device identifier
const char* device_id = "TPA_01";

// ===== Sensor Pin Configuration =====
#define CH4_SENSOR_PIN 32    // ADC pin untuk CH4
#define H2S_SENSOR_PIN 33    // ADC pin untuk H2S
#define TEMP_SENSOR_PIN 34   // ADC pin untuk temperature
#define HUMIDITY_SENSOR_PIN 35 // ADC pin untuk humidity

// ===== GPS Configuration (jika menggunakan GPS module) =====
// Uncomment jika menggunakan GPS
// #define GPS_RX_PIN 16
// #define GPS_TX_PIN 17
// SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

// ===== Global Variables =====
float ch4_ppm = 0;
float h2s_ppm = 0;
float temperature = 0;
float humidity = 0;
float latitude = 0;
float longitude = 0;
bool gps_valid = false;

// ===== Timing =====
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 60000; // Send every 60 seconds

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Configure ADC
  analogReadResolution(12); // 12-bit ADC
  
  // Connect to WiFi
  setupWiFi();
  
  // Initialize GPS (jika diperlukan)
  // setupGPS();
  
  Serial.println("Setup complete!");
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }

  // Read sensor data
  readSensors();
  
  // Send data setiap SEND_INTERVAL
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    sendToAPI();
    lastSendTime = millis();
  }

  delay(1000); // Delay 1 second untuk setiap loop iteration
}

// ===== WiFi Setup =====
void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n✗ WiFi connection failed!");
  }
}

// ===== Read Sensors =====
void readSensors() {
  // Read CH4 sensor
  int ch4_raw = analogRead(CH4_SENSOR_PIN);
  ch4_ppm = mapSensorValue(ch4_raw, 0, 4095, 0, 5000);
  
  // Read H2S sensor
  int h2s_raw = analogRead(H2S_SENSOR_PIN);
  h2s_ppm = mapSensorValue(h2s_raw, 0, 4095, 0, 100);
  
  // Read temperature sensor (contoh DHT22 atau similar)
  int temp_raw = analogRead(TEMP_SENSOR_PIN);
  temperature = mapSensorValue(temp_raw, 0, 4095, 0, 50);
  
  // Read humidity sensor
  int humidity_raw = analogRead(HUMIDITY_SENSOR_PIN);
  humidity = mapSensorValue(humidity_raw, 0, 4095, 0, 100);
  
  // IMPORTANT: Ganti kode di atas dengan library sensor yang sesuai!
  // Contoh untuk DHT22:
  // temperature = dht.readTemperature();
  // humidity = dht.readHumidity();
  
  Serial.printf("CH4: %.2f ppm | H2S: %.2f ppm | Temp: %.2f°C | Humidity: %.2f%%\n",
                ch4_ppm, h2s_ppm, temperature, humidity);
}

// ===== Sensor Value Mapping =====
float mapSensorValue(int rawValue, int minRaw, int maxRaw, float minVal, float maxVal) {
  return minVal + (float)(rawValue - minRaw) / (maxRaw - minRaw) * (maxVal - minVal);
}

// ===== Send Data to API =====
void sendToAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, skipping API call");
    return;
  }

  HTTPClient http;
  
  Serial.println("Sending data to API...");
  
  // Create JSON payload
  StaticJsonDocument<256> doc;
  doc["device_id"] = device_id;
  doc["ch4_ppm"] = ch4_ppm;
  doc["h2s_ppm"] = h2s_ppm;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  doc["gps_valid"] = gps_valid;

  String payload;
  serializeJson(doc, payload);
  
  Serial.println("Payload: " + payload);

  // Send POST request
  http.begin(vercelUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32-SensorNode");

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    Serial.println("Response: " + response);
    
    if (httpResponseCode == 200) {
      Serial.println("✓ Data sent successfully!");
    } else {
      Serial.println("✗ Error in response!");
    }
  } else {
    Serial.printf("✗ Error: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}

// ===== GPS Setup (Optional) =====
/*
void setupGPS() {
  gpsSerial.begin(9600);
  Serial.println("GPS initialized");
}

void updateGPS() {
  // Implement GPS reading logic here
  // Example: Parse NMEA sentences from GPS module
  // Update: latitude, longitude, gps_valid
}
*/

// ===== Troubleshooting =====
/*
Jika data tidak dikirim:
1. Check WiFi connection
2. Verify API URL is correct
3. Check sensor pin configuration
4. Uncomment Serial.println() untuk debugging
5. Monitor API logs di Vercel dashboard
*/
