#include <WiFiNINA.h>
#include <WiFiSSLClient.h>

#define WIFI_SSID       "Krish"
#define WIFI_PASSWORD   "12345678"
#define FIREBASE_HOST   "traffic-lights-eaab9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH   "UAeLQd4YmwFyJXHGWseVr70DwRHw5XrrgnMt7OpO"

const int redPin = 4;
const int yellowPin = 5;
const int greenPin = 6;

WiFiSSLClient client;

void setup() {
  Serial.begin(115200);

  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  Serial.print("Connecting to WiFi");
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  if (fetchLEDStates()) {
    Serial.println("LED states updated.");
  } else {
    Serial.println("Failed to update LED states.");
  }

  delay(1000); // Delay between checks
}

bool fetchLEDStates() {
  if (!client.connect(FIREBASE_HOST, 443)) {
    Serial.println("Connection to Firebase failed.");
    return false;
  }

  // Request all LEDs at once
  String url = "/leds.json?auth=" + String(FIREBASE_AUTH);

  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: " + String(FIREBASE_HOST));
  client.println("User-Agent: ArduinoWiFiNINA");
  client.println("Connection: close");
  client.println();

  // Wait for headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // End of headers
  }

  // Read JSON response body: {"red":true,"yellow":false,"green":true}
  String payload = client.readStringUntil('\n');
  payload.trim();
  Serial.println("Response: " + payload);

  // Simple manual parsing (no need for full JSON lib for this)
  setLEDState(payload, "red", redPin);
  setLEDState(payload, "yellow", yellowPin);
  setLEDState(payload, "green", greenPin);

  client.stop();
  return true;
}

void setLEDState(const String& payload, const char* color, int pin) {
  String key = "\"" + String(color) + "\":true";
  if (payload.indexOf(key) != -1) {
    digitalWrite(pin, HIGH);
    Serial.println(String(color) + " ON");
  } else {
    digitalWrite(pin, LOW);
    Serial.println(String(color) + " OFF");
  }
}
