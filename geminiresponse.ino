#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi Credentials
const char* ssid = "OTTO";          // Wi-Fi SSID
const char* password = "12345678";  // Wi-Fi Password

// Gemini API URL and API Key
const String apiKey = "AIzaSyD51z_HiybRKdNpRw3zou89eN-Rfz_dQ1A"; // Gemini API Key
const String apiURL = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + apiKey;

// Common instruction for Gemini API
const String common = "You are OTTO, a 3D-printed robot developed by ISA-VESIT. OTTO is equipped with servo motors for hand movements, N20 motors for movement, and an OLED display to show expressions. It has an INMP441 microphone for speech recognition, a MAX98357A speaker for speech synthesis, and is powered by an ESP32-S3 DevKit M1 controller for processing. OTTO's capabilities include speech recognition, text processing, and speech synthesis. OTTO can respond to speech, generate outputs, display emotions on the OLED screen, and perform basic movements using its motors. The response you generate will be given for speech synthesis, so provide clear lines without using special characters. Please respond to the next word after this instruction.";

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if there is data available in the serial monitor
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Read the input command
    command.trim(); // Remove any extra spaces or newlines
    if (command.length() > 0) {
      Serial.println("Sending command to Gemini API: " + command);

      // Send command to the Gemini API
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Configure HTTP request
        http.begin(apiURL);
        http.addHeader("Content-Type", "application/json"); // Set content type to JSON

        // Manually construct the JSON payload
        String payload = "{";
        payload += "\"contents\": [{";
        payload += "\"parts\": [{";
        payload += "\"text\": \"" + common + " " + command + "\"";
        payload += "}]";
        payload += "}]";
        payload += "}";

        // Serial.println("Payload: " + payload); // Debug: Print the payload

        // Send POST request
        int httpResponseCode = http.POST(payload);

        // Handle response
        if (httpResponseCode > 0) {
          String response = http.getString();
          // Serial.println("Response from Gemini API: " + response);

          // Manual parsing of the response
          int startIndex = response.indexOf("\"text\": \"") + 9;
          int endIndex = response.indexOf("\"", startIndex + 1);
          String geminiresponse = "";

          if (startIndex > 8 && endIndex > startIndex) {
            geminiresponse = response.substring(startIndex, endIndex);
            Serial.println("Parsed Response: " + geminiresponse); // Print the extracted response
          } else {
            Serial.println("Error: Could not parse the response.");
          }
        } else {
          Serial.println("Error in HTTP request. Code: " + String(httpResponseCode));
        }

        http.end(); // Free resources
      } else {
        Serial.println("Wi-Fi disconnected. Cannot send request.");
      }
    }
  }
}
