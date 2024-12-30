#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi Credentials
const char* ssid = "OTTO";          // Wi-Fi SSID
const char* password = "12345678";  // Wi-Fi Password

// Gemini API URL and API Key
const String apiKey = " "; // Gemini API Key
const String apiURL = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + apiKey;

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
        payload += "\"text\": \"" + command + "\"";
        payload += "}]";
        payload += "}]";
        payload += "}";

        Serial.println("Payload: " + payload); // Debug: Print the payload

        // Send POST request
        int httpResponseCode = http.POST(payload);

        // Handle response
        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("Response from Gemini API: " + response);
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