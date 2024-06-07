#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h> // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library


LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD with I2C address 0x27, 16 columns, and 2 rows

float latitude, longitude; // create variables for latitude and longitude

SoftwareSerial gpsSerial(8, 9); // rx, tx
TinyGPSPlus gps; // create GPS object

bool gpsDataReceived = false;
SoftwareSerial espSerial(2, 3); // RX, TX
// Replace with your ThingSpeak API key
String apiKey = "FUFDBB3GY8FD6AO7";
const long channelID = 2566267;

void setup() {
  lcd.begin(16, 2);

  gpsSerial.begin(115200); // connect GPS sensor

  lcd.print("GPS TRACKER");

  delay(2000);

  lcd.clear();
  // Start the hardware serial port
  Serial.begin(115200);

  // Start the software serial port for ESP8266
  espSerial.begin(115200);

  // Wait for serial ports to initialize
  delay(1000);

  // Reset the ESP8266
  sendATCommand("AT+RST", 1000, true);

  // Set Wi-Fi mode to Station (Client)
  sendATCommand("AT+CWMODE=1", 1000, true);

  // Connect to Wi-Fi network
  String ssid = "Dark boy";
  String password = "test12345";
  sendATCommand("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"", 5000, true);

}

void loop() {
  float longitude = random(0, 100);
  float latitude = random(0, 200);
  String cordinates = String(longitude)+","+String(latitude);
  // Continuously read data from ESP8266 and print to Serial Monitor
  if (espSerial.available()) {
    Serial.write(espSerial.read());
  }
  if (Serial.available()) {
    espSerial.write(Serial.read());
  }
  // function to send data to thingspeak server.
   sendDataToThingSpeak(latitude,longitude);
}
void sendDataToThingSpeak(float lat,float log) {
  // Send data to ThingSpeak
  String data = "GET /update?api_key=" + apiKey + "&field1=" +lat + "&field2=" + log;
  sendATCommand("AT+CIPMUX=1", 1000, "OK");
  sendATCommand("AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",80", 1000, true);
  sendATCommand("AT+CIPSEND=0," + String(data.length() + 4), 1000, true);
  espSerial.println(data);
  delay(1000);
  sendATCommand("AT+CIPCLOSE=0", 1000, true);
  Serial.print("----------- Done sending to server --------------------");
  delay(1000); // Wait 15 seconds before sending the next data
}
void sendATCommand(String command, const int timeout, boolean debug) {
  
  espSerial.println(command);  
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (espSerial.available()) {
      char c = espSerial.read();
      if (debug) {
        Serial.write(c);
      }
    }
  }
}

