🌱 Automatic Plant Monitoring & Smart Irrigation System
🚀 An IoT-Based Smart Farming Solution
⚠️ This is an IoT project and cannot be tested without hardware.

📌 Overview
This project is a smart agriculture system that automates plant monitoring and irrigation using IoT, AI, and Cloud. It measures soil moisture, temperature, and humidity, controls a relay-based water pump, logs data to Google Sheets, and integrates live weather updates from OpenWeatherMap.

🔧 Features
✅ Automatic Soil Moisture Monitoring – Detects soil dryness and activates irrigation
✅ Weather API Integration – Fetches live temperature & humidity data
✅ Google Sheets Logging – Stores real-time sensor data for analysis
✅ Motion Detection (PIR Sensor) – Alerts when motion is detected
✅ Remote Monitoring & Control – Manage everything from the Blynk IoT Dashboard
✅ LCD Display (I2C) – Real-time status display

🛠️ Technologies Used
📡 IoT & Communication:
ESP8266 NodeMCU – WiFi-enabled microcontroller
Blynk IoT – Remote monitoring & control
HTTP & REST APIs – Google Sheets & Weather API integration
Wi-Fi Communication – Connects sensors to the internet
📊 Cloud & Data Logging:
Google Sheets API – Logs sensor data for long-term analysis
Google Apps Script – Automates cloud data entry
🔧 Hardware Components:
Soil Moisture Sensor – Monitors water levels in soil
DHT11 Sensor – Measures temperature & humidity
PIR Motion Sensor – Detects motion for security alerts
Relay Module – Controls water pump for automated irrigation
LCD Display (I2C) – Displays real-time sensor data
🖥️ Blynk IoT Dashboard Setup
1️⃣ Add Widgets in Blynk:

V0, V1 → Temperature & Humidity
V3 → Soil Moisture
V10, V11 → Weather Data (Temperature & Humidity)
V12 → Manual Motor Control
V20, V21 → City & Country Input
2️⃣ Configure Virtual Pins in the code to match Blynk

🚀 How to Set Up & Run
1️⃣ Hardware Setup
Connect the components as follows:

Component	ESP8266 Pin
Soil Sensor	A0
Relay (Pump)	D3
PIR Sensor	D5
DHT11 Sensor	D4
LCD (I2C)	D1, D2
2️⃣ Google Sheets Setup
Open Google Sheets → Go to Extensions → Apps Script
Copy & Paste the Google Apps Script (from the project code)
Deploy as Web App → Set Access: Anyone
Copy the Webhook URL
3️⃣ Upload Code to ESP8266
Update WiFi & Blynk Credentials in the code
Replace Webhook URL in googleScriptUrl
Upload & Open Serial Monitor
📢 Important Notes
⚠️ This is an IoT project and cannot be tested without hardware.
⚠️ Google Sheets has a request limit; sending data every 1 second may cause issues.
⚠️ Ensure Blynk & OpenWeatherMap API keys are correct before uploading.

👨‍💻 Contributing
Feel free to fork this project and improve it! Contributions are always welcome.

📜 License
This project is open-source under the MIT License.

🚀 Built for Smart Agriculture & IoT Automation! 🌱
