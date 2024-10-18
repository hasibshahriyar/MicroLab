#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// Motor Driver and Servo Pin Definitions
#define ENA 12   // Enable pin for motor 1
#define ENB 12   // Enable pin for motor 2 (same pin as ENA in your config)
#define IN1 13   // Input pin 1 for motor 1
#define IN2 15   // Input pin 2 for motor 1
#define IN3 14   // Input pin 3 for motor 2
#define IN4 2    // Input pin 4 for motor 2
#define SERVO_PIN 0  // Pin for the steering servo

// WiFi Credentials
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

// Server URL (Make sure the IP is your server's IP address in the local network)
const char* serverUrl = "http://192.168.1.10:5000/process_video";

// Define Servo object
Servo steeringServo;
int servoPosition = 90;  // Default position (centered)

// Initialize WiFi and HTTP Client
WiFiClient client;
HTTPClient http;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  
  // Motor pins setup
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Attach Servo Motor to pin
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(servoPosition);  // Center the steering

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi.");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Capture camera frame
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Send frame buffer to server for processing
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(fb->buf, fb->len);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
      // Process server's response (left, right, forward, stop)
      if (response == "left") {
        turnLeft();
      } else if (response == "right") {
        turnRight();
      } else if (response == "forward") {
        moveForward();
      } else if (response == "stop") {
        stopCar();
      }
    } else {
      Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    // Clean up
    http.end();
    esp_camera_fb_return(fb);
  }

  delay(100);  // Adjust this for real-time operation
}

// Motor Control Functions (based on the L298 driver)

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Set motor speed (using PWM)
  analogWrite(ENA, 100);  // Full speed for motor 1
  analogWrite(ENB, 90);  // Full speed for motor 2
}

void turnLeft() {
  steeringServo.write(servoPosition - 30);  // Turn servo to the left
  moveForward();
}

void turnRight() {
  steeringServo.write(servoPosition + 30);  // Turn servo to the right
  moveForward();
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Stop the motors
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}