#include <WiFiNINA.h>
#include <Firebase_Arduino_WiFiNINA.h>

#define WIFI_SSID "Krish"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "traffic-lights-eaab9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UAeLQd4YmwFyJXHGWseVr70DwRHw5XrrgnMt7OpO" // Get this from Firebase console

const int redPin = 4;
const int yellowPin = 5;
const int greenPin = 6;

FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  
  // Initialize LED pins
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // Connect to WiFi
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
}

void loop() {
  updateLED("red", redPin);
  updateLED("yellow", yellowPin);
  updateLED("green", greenPin);
  
  delay(1000);
}

void updateLED(String color, int pin) {
  String path = "/leds/" + color;
  
  if (Firebase.getBool(firebaseData, path)) {
    if (firebaseData.dataType() == "boolean") {
      bool value = firebaseData.boolData();
      digitalWrite(pin, value ? HIGH : LOW);
    }
  } 
}