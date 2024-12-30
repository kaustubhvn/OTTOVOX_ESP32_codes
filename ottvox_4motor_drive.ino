#include <WiFi.h>
#include <WebServer.h>
#include <math.h> // For sin() and cos()
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>  
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64  
#define OLED_RESET -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Eye parameters
int eye_height = 40;
int eye_width = 40;
int radius = 15;
int x = 44;
int y = 12;

// Heart parameters
int heartSize = 10;
int maxSize = 30;
int minSize = 10;
bool growing = true;
int step = 2;

// Dotted eye parameters
int centerX = SCREEN_WIDTH / 2; // Horizontal center of the screen
int centerY = SCREEN_HEIGHT / 2; // Vertical center of the screen
int radiusX = 25;  // Horizontal radius (wider for a slightly wider oval)
int radiusY = 30;  // Vertical radius (keeps the vertical height)
int circleRadius = 8; // Radius of the circle

int verticalSpeed, horizontalSpeed;


const char* ssid = "Hrishi";
const char* password = "krishhrish";

WebServer server(80);

// HTML code for the web page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Slider Control</title>
</head>
<body>
  <h1>ESP32 Slider Control</h1>
  
  <!-- Vertical Slider -->
  <h2>Forward and Backward</h2>
  <input type="range" min="0" max="255" value="0" id="verticalSlider" oninput="sendVerticalValue(this.value)">
  <p>Direction: <span id="verticalDirection">Stop</span></p>
  
  <!-- Horizontal Slider -->
  <h2>Left and Right</h2>
  <input type="range" min="0" max="255" value="0" id="horizontalSlider" oninput="sendHorizontalValue(this.value)">
  <p>Direction: <span id="horizontalDirection">Stop</span></p>

  <script>
    function sendVerticalValue(value) {
      let direction = "Stop";
      if (value > 138) direction = "Forward";
      else if (value < 120) direction = "Backward";

      document.getElementById('verticalDirection').innerText = direction;
      fetch(`/vertical?value=${value}`, { method: 'GET', cache: 'no-cache' });
    }

    function sendHorizontalValue(value) {
      let direction = "Stop";
      if (value > 138) direction = "Right";
      else if (value < 120) direction = "Left";

      document.getElementById('horizontalDirection').innerText = direction;
      fetch(`/horizontal?value=${value}`, { method: 'GET', cache: 'no-cache' });
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleVertical() {
  if (server.hasArg("value")) {
    int verticalValue = server.arg("value").toInt();
    Serial.println("Vertical Slider Value: " + String(verticalValue));

    if (verticalValue >= 138) {
      verticalSpeed = map(verticalValue, 138, 255, 0, 255); 
      Serial.println("Forward");
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(9, HIGH);
      digitalWrite(11, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(5, LOW);
      digitalWrite(8, LOW);
      digitalWrite(10, LOW);
      analogWrite(1, verticalSpeed);
      analogWrite(6, verticalSpeed);
      analogWrite(7, verticalSpeed);
      analogWrite(12, verticalSpeed);
    } else if (verticalValue <= 120) {
      verticalSpeed = map(verticalValue, 120, 0, 0, 255);
      Serial.println("Backward");
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(9, LOW);
      digitalWrite(11, LOW);
      digitalWrite(2, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(10, HIGH);
      analogWrite(1, verticalSpeed);
      analogWrite(6, verticalSpeed);
      analogWrite(7, verticalSpeed);
      analogWrite(12, verticalSpeed);
    } else {
      Serial.println("Stop");
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(9, LOW);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);
      digitalWrite(5, LOW);
      digitalWrite(8, LOW);
      digitalWrite(10, LOW);
      analogWrite(1, 0);
      analogWrite(6, 0);
      analogWrite(7, 0);
      analogWrite(12, 0);
    }
  }
  server.send(200, "text/plain", "Vertical Value received");
}

const unsigned char myBitmap_1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
	0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
	0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xe0, 0x00, 
	0x00, 0x03, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xe0, 0x00, 
	0x00, 0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf8, 0x00, 
	0x00, 0x0f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x3f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x00, 
	0x00, 0x7f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x00, 
	0x00, 0x7f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x00, 
	0x00, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0x00, 
	0x00, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 
	0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 
	0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 
	0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x00, 
	0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 
	0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void handleHorizontal() {
  if (server.hasArg("value")) {
    int horizontalValue = server.arg("value").toInt();
    Serial.println("Horizontal Slider Value: " + String(horizontalValue));

    if (horizontalValue >= 138) {
      horizontalSpeed = map(horizontalValue, 138, 255, 0, 255);
      Serial.println("Right");
      digitalWrite(4, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);
      digitalWrite(8, HIGH);
      digitalWrite(5,LOW);
      digitalWrite(10,HIGH);
      digitalWrite(3,HIGH);
      digitalWrite(9,LOW);
      analogWrite(1, horizontalSpeed);
      analogWrite(6, horizontalSpeed);
      analogWrite(7, horizontalSpeed);
      analogWrite(12, horizontalSpeed);
    } else if (horizontalValue <= 120) {
      horizontalSpeed = map(horizontalValue, 120, 0, 0, 255);
      Serial.println("Left");
      digitalWrite(4, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(5,HIGH);
      digitalWrite(10,LOW);
      digitalWrite(3,LOW);
      digitalWrite(9,HIGH);
      analogWrite(1, horizontalSpeed);
      analogWrite(6, horizontalSpeed);
      analogWrite(7, horizontalSpeed);
      analogWrite(12, horizontalSpeed);
    } else {
      Serial.println("Stop");
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(9, LOW);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);
      digitalWrite(5, LOW);
      digitalWrite(8, LOW);
      digitalWrite(10, LOW);
      analogWrite(1, 0);
      analogWrite(6, 0);
      analogWrite(7, 0);
      analogWrite(12, 0);
    }
  }
  server.send(200, "text/plain", "Horizontal Value received");
}

void setup() {
  
  Serial.begin(115200);
  Serial.begin(115200);
  Wire.begin(14,13);
  if (!display.begin(0x3C, OLED_RESET)) {  
    for (;;); 
  }
  display.clearDisplay();
  

  
  display.drawBitmap(0, 0, myBitmap_1, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);

  display.display();
  delay(1000);

  display.clearDisplay(); 
 

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up server routes
  server.on("/", handleRoot);
  server.on("/vertical", handleVertical);
  server.on("/horizontal", handleHorizontal);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
  
  //back motors
  pinMode(1, OUTPUT); // PWMA
  pinMode(2, OUTPUT); // AI2
  pinMode(3, OUTPUT); // AI1
  
  pinMode(6, OUTPUT); // PWMB
  pinMode(5, OUTPUT); // BI2
  pinMode(4, OUTPUT); // BI1

  //front motors
  pinMode(7, OUTPUT); // PWMA
  pinMode(8, OUTPUT); // AI2
  pinMode(9, OUTPUT); // AI1
  
  pinMode(12, OUTPUT); // PWMB
  pinMode(10, OUTPUT); // BI2
  pinMode(11, OUTPUT); // BI1
}



void loop() {
  server.handleClient();

  
}
