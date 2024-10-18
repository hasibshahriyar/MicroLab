#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"

// Replace with your network credentials
const char* ssid = "Your_SSID"; // Replace with your Wi-Fi SSID
const char* password = "Your_PASSWORD"; // Replace with your Wi-Fi password

// Motor pins for L298N motor driver
const int ENA = 12;   // Enable pin for left motor (PWM) on GPIO12
const int ENB = 12;   // Enable pin for right motor (PWM) on GPIO12
const int IN1 = 13;   // Left motor forward
const int IN2 = 15;   // Left motor backward
const int IN3 = 14;   // Right motor forward
const int IN4 = 2;    // Right motor backward

// Create an instance of the WebServer
WebServer server(80);

// Camera pin configuration
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Set up PWM channels for motor speed control
const int PWM_CHANNEL_LEFT = 0;   // Left motor PWM channel
const int PWM_CHANNEL_RIGHT = 1;  // Right motor PWM channel
const int PWM_FREQUENCY = 5000;   // PWM frequency
const int PWM_RESOLUTION = 8;     // 8-bit resolution (0-255)

void setupMotorPins() {
    // Motor control pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    
    // Set up PWM channels for speed control
    ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(ENA, PWM_CHANNEL_LEFT);  // Left motor PWM channel
    
    ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(ENB, PWM_CHANNEL_RIGHT); // Right motor PWM channel

    ledcWrite(PWM_CHANNEL_LEFT, 0);  // Initialize left motor speed to 0
    ledcWrite(PWM_CHANNEL_RIGHT, 0); // Initialize right motor speed to 0

    stopCar();  // Start with motors stopped
}

// Motor control functions with speed limit
void moveForward() {
    ledcWrite(PWM_CHANNEL_LEFT, 60); // Limit speed to 60 (0-255)
    ledcWrite(PWM_CHANNEL_RIGHT, 60); // Limit speed to 60 (0-255)
    digitalWrite(IN1, HIGH);  
    digitalWrite(IN2, LOW);   
    digitalWrite(IN3, HIGH);  
    digitalWrite(IN4, LOW);   
}

void moveBackward() {
    ledcWrite(PWM_CHANNEL_LEFT, 60); // Limit speed to 60 (0-255)
    ledcWrite(PWM_CHANNEL_RIGHT, 60); // Limit speed to 60 (0-255)
    digitalWrite(IN1, LOW);  
    digitalWrite(IN2, HIGH);  
    digitalWrite(IN3, LOW);  
    digitalWrite(IN4, HIGH);
}

void turnLeft() {
    ledcWrite(PWM_CHANNEL_LEFT, 60); // Limit speed to 60 (0-255)
    ledcWrite(PWM_CHANNEL_RIGHT, 60); // Limit speed to 60 (0-255)
    digitalWrite(IN1, LOW);   
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);  
    digitalWrite(IN4, LOW);
}

void turnRight() {
    ledcWrite(PWM_CHANNEL_LEFT, 60); // Limit speed to 60 (0-255)
    ledcWrite(PWM_CHANNEL_RIGHT, 60); // Limit speed to 60 (0-255)
    digitalWrite(IN1, HIGH);  
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void stopCar() {
    digitalWrite(IN1, LOW);  // Stop left motor
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);  // Stop right motor
    digitalWrite(IN4, LOW);
    ledcWrite(PWM_CHANNEL_LEFT, 0);  // Stop left motor PWM
    ledcWrite(PWM_CHANNEL_RIGHT, 0); // Stop right motor PWM
}

// Camera initialization
void initCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size = FRAMESIZE_QVGA; // Small frame size for better performance
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }
}

// Web server route for live camera feed
void handleCameraFeed() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        server.send(503, "text/plain", "Camera capture failed");
        return;
    }
    // Send response header with content length and type
    server.setContentLength(fb->len);
    server.send(200, "image/jpeg", "");
    
    // Send the actual image data
    WiFiClient client = server.client();
    client.write(fb->buf, fb->len);
    
    esp_camera_fb_return(fb);
}

// Web page to control motors and show camera feed
void handleRoot() {
    String html = "<html><body>";
    html += "<h1>ESP32-CAM Motor Control & Live Feed</h1>";
    html += "<img src='/camera' /><br>";
    html += "<button onclick=\"location.href='/forward'\">Forward</button><br>";
    html += "<button onclick=\"location.href='/backward'\">Backward</button><br>";
    html += "<button onclick=\"location.href='/left'\">Left</button><br>";
    html += "<button onclick=\"location.href='/right'\">Right</button><br>";
    html += "<button onclick=\"location.href='/stop'\">Stop</button><br>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize motor pins
    setupMotorPins();

    // Initialize camera
    initCamera();

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
    Serial.print("Server IP address: ");
    Serial.println(WiFi.localIP());

    // Define routes
    server.on("/", handleRoot);
    server.on("/camera", HTTP_GET, handleCameraFeed);
    server.on("/forward", HTTP_GET, []() {
        moveForward();
        server.send(200, "text/plain", "Moving forward");
    });
    server.on("/backward", HTTP_GET, []() {
        moveBackward();
        server.send(200, "text/plain", "Moving backward");
    });
    server.on("/left", HTTP_GET, []() {
        turnLeft();
        server.send(200, "text/plain", "Turning left");
    });
    server.on("/right", HTTP_GET, []() {
        turnRight();
        server.send(200, "text/plain", "Turning right");
    });
    server.on("/stop", HTTP_GET, []() {
        stopCar();
        server.send(200, "text/plain", "Stopping");
    });
    
    // Start the server
    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
}