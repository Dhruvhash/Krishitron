/* Connections
Relay. D3
Btn.   D7
Soil.  A0
PIR.   D5
SDA.   D2
SCL.   D1
Temp.  D4
*/

// Add required Blynk template information
#define BLYNK_TEMPLATE_ID "TMPL30dHL9BZ8"
#define BLYNK_TEMPLATE_NAME "plant monitoring"
#define BLYNK_AUTH_TOKEN "QXZf9xG7PNgDt-dzwoqSN5p0-6a2oTIX"
#define BLYNK_PRINT Serial

// Include the library files
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// OpenWeatherMap API Key
const char* apiKey = "d1e6f44c1829745ba12a3eb9e4efb132";
String city = "Delhi";   // Default city
String country = "IN";   // Default country

// Use the auth token from the define
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Dhruv";  // Enter your WiFi SSID
char pass[] = "dhruv@2006";  // Enter your WiFi Password

// Google Sheets API - replace with your Web App URL from Google Apps Script
const char* googleSheetsURL = "https://script.google.com/macros/s/AKfycbzOHyYhJKoozwhySV1EqadPcasskPdKw7lGt9pTzgj_lNYlpH3Mn7wfL83atsuOZPHmmQ/exec";

DHT dht(D4, DHT11);  // D4 - DHT11 Temperature Sensor
BlynkTimer timer;
WiFiClient client;
WiFiClientSecure secureClient;

// Define component pins
#define soil A0     // A0 - Soil Moisture Sensor
#define PIR D5      // D5 - PIR Motion Sensor
int PIR_ToggleValue;

// Variables to store sensor data
float temperature = 0;
float humidity = 0;
int soilMoisture = 0;
bool motionDetected = false;
float weatherTemp = 0;
float weatherHumidity = 0;

void checkPhysicalButton();
int relay1State = LOW;
int pushButton1State = HIGH;
#define RELAY_PIN_1       D3   // D3 - Relay
#define PUSH_BUTTON_1     D7   // D7 - Button
#define VPIN_BUTTON_1     V12  

// Virtual Pins for Weather API Data
#define VIRTUAL_TEMPERATURE V10
#define VIRTUAL_HUMIDITY V11
#define VIRTUAL_CITY V20
#define VIRTUAL_COUNTRY V21

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(PIR, INPUT);

  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");

  // Call the function
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, checkPhysicalButton);
  timer.setInterval(120000L, fetchWeatherData);  // Fetch Weather Every 2 Minutes
  timer.setInterval(60000L, sendDataToGoogleSheets);  // Send data to Google Sheets every 1 minute
  
  // SSL is required for Google services but we'll skip verification for ESP8266
  secureClient.setInsecure();
}

// Get the DHT11 sensor values
void DHT11sensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(humidity);
}

// Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;
  soilMoisture = value;

  Blynk.virtualWrite(V3, value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print(" ");
}

// Get the PIR sensor values
void PIRsensor() {
  motionDetected = digitalRead(PIR);
  if (motionDetected) {
    Blynk.logEvent("pirmotion", "WARNING! Motion Detected!");
    WidgetLED LED(V5);
    LED.on();
  } else {
    WidgetLED LED(V5);
    LED.off();
  }
}

// Send data to Google Sheets
void sendDataToGoogleSheets() {
  Serial.println("Sending data to Google Sheets...");
  
  // Format data as URL parameters
  String url = String(googleSheetsURL);
  url += "?temperature=" + String(temperature);
  url += "&humidity=" + String(humidity);
  url += "&soil=" + String(soilMoisture);
  url += "&motion=" + String(motionDetected ? "Yes" : "No");
  url += "&relay=" + String(relay1State ? "ON" : "OFF");
  url += "&weatherTemp=" + String(weatherTemp);
  url += "&weatherHumidity=" + String(weatherHumidity);
  
  HTTPClient https;
  
  Serial.print("Connecting to ");
  Serial.println(url);
  
  if (https.begin(secureClient, url)) {
    int httpCode = https.GET();
    
    if (httpCode > 0) {
      Serial.printf("Response code: %d\n", httpCode);
      
      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString();
        Serial.println("Google Sheets response: ");
        Serial.println(payload);
      }
    } else {
      Serial.printf("Request failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    
    https.end();
  } else {
    Serial.println("Unable to connect to Google Sheets");
  }
}

BLYNK_WRITE(V6) {
  PIR_ToggleValue = param.asInt();
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}

// Fetch Weather Data from OpenWeatherMap API
void fetchWeatherData() {
    String requestURL = "/data/2.5/weather?q=" + city + "," + country + "&appid=" + String(apiKey) + "&units=metric";
    HTTPClient http;
    http.begin(client, "http://api.openweathermap.org" + requestURL);

    int httpCode = http.GET();
    if (httpCode == 200) {
        String response = http.getString();
        Serial.println("Weather Data Received!");

        // Parse JSON response
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

        weatherTemp = doc["main"]["temp"];
        weatherHumidity = doc["main"]["humidity"];

        Serial.print("Weather Temp: "); Serial.print(weatherTemp); Serial.println("°C");
        Serial.print("Weather Humidity: "); Serial.print(weatherHumidity); Serial.println("%");

        // Send Weather Data to Blynk
        Blynk.virtualWrite(VIRTUAL_TEMPERATURE, weatherTemp);
        Blynk.virtualWrite(VIRTUAL_HUMIDITY, weatherHumidity);
    } else {
        Serial.print("Weather API Error: ");
        Serial.println(httpCode);
    }
    http.end();
}

// Blynk Handler: User Inputs City Name
BLYNK_WRITE(VIRTUAL_CITY) {
    city = param.asStr();
    Serial.print("Updated City: ");
    Serial.println(city);
}

// Blynk Handler: User Inputs Country Code
BLYNK_WRITE(VIRTUAL_COUNTRY) {
    country = param.asStr();
    Serial.print("Updated Country: ");
    Serial.println(country);
}

void checkPhysicalButton() {
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    if (pushButton1State != LOW) {
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
}

void loop() {
  if (PIR_ToggleValue == 1) {
    lcd.setCursor(5, 1);
    lcd.print("M:ON ");
    PIRsensor();
  } else {
    lcd.setCursor(5, 1);
    lcd.print("M:OFF");
    WidgetLED LED(V5);
    LED.off();
  }

  if (relay1State == HIGH) {
    lcd.setCursor(11, 1);
    lcd.print("W:ON ");
  } else {
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
  }

  Blynk.run();
  timer.run();
}
