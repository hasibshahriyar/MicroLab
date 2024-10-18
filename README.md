# MicroLab
Line Following Car using ESP32-CAM Module:

Project Overview-
    This project involves building an autonomous line-following car using the ESP32-CAM module for real-time video       
    processing and an L298N motor driver to control the car's movement. The vehicle detects and follows a black line on a 
    surface using visual input from the camera. It processes this video feed in real time, adjusting its motors accordingly 
    to maintain a straight path.

Features-
    Camera-based Line Detection: Utilizes the ESP32-CAM module to capture a real-time video feed of the path.
    Motor Control: The L298N motor driver is used to control the car's left and right motors for steering and speed 
    adjustment.
    Real-time Processing: Video is processed on the ESP32-CAM module to determine the line's position, adjust motor 
    controls for autonomous navigation.
    Adaptability: The system can adjust motor speed based on the path's curvature, improving performance over traditional IR 
    sensor-based systems.

Components-
    ESP32-CAM: Captures the video feed and processes it for line detection.
    L298N Motor Driver: Controls the two DC motors (left and right) of the car.
    DC Motors: Used to drive the wheels of the car.
    Chassis: Platform that houses all the components.
    Power Supply: Provides necessary power for ESP32 and motors

Wiring Diagram-
1. ESP32-CAM Module:
    Connect the camera module to the chassis.
    VCC and GND of the ESP32-CAM connected to the power source.
    Motor control pins connected to the L298N driver.

2. L298N Motor Driver:
    IN1, IN2 connected to the motor inputs.
    OUT1 and OUT2 connected to the left motor, OUT3 and OUT4 to the right motor.
    Power inputs connected to the battery.

Installation-
1. Arduino IDE Setup:
    Download and install the Arduino IDE.
    Add the ESP32 board to your Arduino IDE by following this guide.
    Install the necessary libraries, such as WiFi, ESP32-CAM, and Motor Control.

2. Code Upload:
    Write or upload the provided code for the line-following algorithm.
    Ensure the ESP32-CAM is connected to the correct serial port for uploading.
    Adjust parameters in the code such as motor speed and PID control variables based on your environment.

3. Wiring:
    Connect the ESP32-CAM and L298N Motor Driver to the motors and power supply as described in the wiring section.
    Double-check connections to avoid short circuits or wiring errors.

How it Works-
1. Camera Input:
    The ESP32-CAM captures video of the path.
    The video feed is processed to detect the black line on a white background using image thresholding.

2. Motor Control:
    Based on the detected position of the line, the ESP32 adjusts the motor speed and direction through the L298N driver.
    The car steers left or right depending on the deviation of the line from the center of the camera feed.

3. Autonomous Navigation:
    The system continually processes the video feed to ensure the car stays on track.
    Adjustments are made dynamically to account for changes in lighting or path curvature.

Known Limitations-
    Wi-Fi Latency: Video processing might experience latency if the Wi-Fi signal is weak, leading to delays in the car's response.
    Lighting Variability: Significant changes in lighting conditions may affect the accuracy of line detection.
    Blind Spots: The single camera's limited field of view can create blind spots where the line may not be detected.

Future Improvements-
    Implementing obstacle detection using additional sensors.
    Optimizing video processing algorithms for faster and more reliable line detection.
    Adding GPS for outdoor navigation and mapping.

License-
    This project is open-source and free to use under the MIT License.
