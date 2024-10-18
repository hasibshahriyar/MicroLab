from flask import Flask, request, jsonify
import numpy as np
import cv2
import io

app = Flask(__name__)

# Line detection threshold values (adjust these based on your environment)
LINE_COLOR_LOWER = np.array([0, 0, 0], dtype="uint8")  # Lower HSV threshold for the line color (black)
LINE_COLOR_UPPER = np.array([180, 255, 50], dtype="uint8")  # Upper HSV threshold for the line color (black)
frame_width = 640  # Width of the video frame (modify based on your camera resolution)

@app.route('/process_video', methods=['POST'])
def process_video():
    # Receive the image frame from the ESP32
    image_data = request.data
    np_arr = np.frombuffer(image_data, np.uint8)
    frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

    if frame is None:
        return "error", 400

    # Resize the frame for faster processing (optional)
    resized_frame = cv2.resize(frame, (frame_width, int(frame.shape[0] * (frame_width / frame.shape[1]))))

    # Convert frame to HSV color space for easier color detection
    hsv_frame = cv2.cvtColor(resized_frame, cv2.COLOR_BGR2HSV)

    # Threshold the HSV frame to get the line color
    mask = cv2.inRange(hsv_frame, LINE_COLOR_LOWER, LINE_COLOR_UPPER)

    # Find contours in the mask to detect the line
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    if len(contours) > 0:
        # Get the largest contour, which should be the line
        largest_contour = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(largest_contour)

        # Calculate the centroid of the detected line
        cx = x + w // 2
        cy = y + h // 2

        # Determine direction based on the centroid position
        if cx < frame_width * 0.4:
            direction = "left"
        elif cx > frame_width * 0.6:
            direction = "right"
        else:
            direction = "forward"
    else:
        direction = "stop"  # Stop if no line is detected

    print(f"Direction: {direction}")
    return direction  # Send the movement direction back to the ESP32

@app.route('/')
def index():
    return "ESP32 Camera Line Tracking Server"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
