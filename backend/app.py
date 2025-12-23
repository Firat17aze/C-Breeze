#!/usr/bin/env python3
"""
IoT Smart Fan Control System - Computer Vision Module (Face Tracking)
Detects humans in close range and communicates with Arduino via serial.
"""

import cv2
import serial
import time
import json
import requests
import threading
from typing import Optional, Tuple
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class VisionDetector:
    def __init__(self, config_file: str = 'vision_config.json'):
        """Initialize the vision detector with configuration."""
        self.load_config(config_file)
        self.serial_conn: Optional[serial.Serial] = None
        self.camera: Optional[cv2.VideoCapture] = None
        self.running = False
        self.current_mode = 'AUTO'  # AUTO or MANUAL
        self.human_detected = False
        self.detection_confidence = 0.0
        
        # Load Haar Cascade for face detection
        try:
            self.face_cascade = cv2.CascadeClassifier(
                cv2.data.haarcascades + 'haarcascade_frontalface_default.xml'
            )
            if self.face_cascade.empty():
                raise ValueError("Failed to load Haar Cascade")
        except Exception as e:
            logger.error(f"Error loading Haar Cascade: {e}")
            raise
        
        # Initialize serial connection
        self.init_serial()
        
        # Initialize camera
        self.init_camera()
    
    def load_config(self, config_file: str):
        """Load configuration from JSON file."""
        default_config = {
            "serial_port": "COM3",
            "baud_rate": 9600,
            "camera_index": 0,
            "detection_threshold": 0.3,
            "close_range_face_size": 100,  # Minimum face size in pixels for close range
            "frame_width": 640,
            "frame_height": 480,
            "fps": 30,
            "dashboard_url": "http://localhost:3001",
            "websocket_url": "ws://localhost:3001"
        }
        
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
                self.config = {**default_config, **config}
        except FileNotFoundError:
            logger.warning(f"Config file {config_file} not found. Using defaults.")
            self.config = default_config
            self.save_config(config_file)
    
    def save_config(self, config_file: str):
        """Save current configuration to JSON file."""
        with open(config_file, 'w') as f:
            json.dump(self.config, f, indent=2)
    
    def init_serial(self):
        """Initialize serial connection to Arduino."""
        try:
            self.serial_conn = serial.Serial(
                port=self.config['serial_port'],
                baudrate=self.config['baud_rate'],
                timeout=1,
                write_timeout=1
            )
            time.sleep(2)  # Wait for Arduino to initialize
            logger.info(f"Serial connection established on {self.config['serial_port']}")
        except serial.SerialException as e:
            logger.error(f"Failed to open serial port: {e}")
            logger.error("Please check the port name and ensure Arduino is connected.")
            raise
    
    def init_camera(self):
        """Initialize camera capture."""
        try:
            self.camera = cv2.VideoCapture(self.config['camera_index'])
            if not self.camera.isOpened():
                raise ValueError("Camera not accessible")
            
            # Set camera properties
            self.camera.set(cv2.CAP_PROP_FRAME_WIDTH, self.config['frame_width'])
            self.camera.set(cv2.CAP_PROP_FRAME_HEIGHT, self.config['frame_height'])
            self.camera.set(cv2.CAP_PROP_FPS, self.config['fps'])
            
            logger.info("Camera initialized successfully")
        except Exception as e:
            logger.error(f"Failed to initialize camera: {e}")
            raise
    
    def detect_human(self, frame) -> Tuple[bool, float, int]:
        """
        Detect human faces in the frame.
        Returns: (detected, confidence, largest_face_size)
        """
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        faces = self.face_cascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30)
        )
        
        if len(faces) > 0:
            # Find the largest face (closest person)
            largest_face = max(faces, key=lambda x: x[2] * x[3])
            x, y, w, h = largest_face
            face_size = max(w, h)
            confidence = min(1.0, face_size / self.config['close_range_face_size'])
            
            # Check if face is large enough (close range)
            is_close = face_size >= self.config['close_range_face_size']
            
            return is_close, confidence, face_size
        
        return False, 0.0, 0
    
    def send_to_arduino(self, command: str):
        """Send command to Arduino via serial."""
        if self.serial_conn and self.serial_conn.is_open:
            try:
                self.serial_conn.write(command.encode('ascii'))
                logger.debug(f"Sent to Arduino: {command}")
            except Exception as e:
                logger.error(f"Error sending to Arduino: {e}")
    
    def send_to_dashboard(self, data: dict):
        """Send detection status to dashboard via HTTP."""
        try:
            response = requests.post(
                f"{self.config['dashboard_url']}/api/vision-update",
                json=data,
                timeout=0.5
            )
        except requests.exceptions.RequestException:
            # Dashboard might not be running, silently fail
            pass
    
    def read_arduino_status(self):
        """Read status messages from Arduino."""
        if self.serial_conn and self.serial_conn.in_waiting > 0:
            try:
                line = self.serial_conn.readline().decode('ascii', errors='ignore').strip()
                if line:
                    logger.debug(f"Received from Arduino: {line}")
                    return line
            except Exception as e:
                logger.error(f"Error reading from Arduino: {e}")
        return None
    
    def set_mode(self, mode: str):
        """Set operating mode (AUTO or MANUAL)."""
        if mode in ['AUTO', 'MANUAL']:
            self.current_mode = mode
            if mode == 'MANUAL':
                self.send_to_arduino('M')
            else:
                self.send_to_arduino('O')
            logger.info(f"Mode changed to: {mode}")
    
    def run(self):
        """Main detection loop."""
        self.running = True
        logger.info("Starting vision detection loop...")
        
        frame_count = 0
        last_status_send = time.time()
        
        try:
            while self.running:
                # Read frame from camera
                ret, frame = self.camera.read()
                if not ret:
                    logger.warning("Failed to read frame from camera")
                    time.sleep(0.1)
                    continue
                
                # Only detect in AUTO mode
                if self.current_mode == 'AUTO':
                    detected, confidence, face_size = self.detect_human(frame)
                    self.human_detected = detected
                    self.detection_confidence = confidence
                    
                    # Send activation signal to Arduino
                    if detected:
                        self.send_to_arduino('1')
                    else:
                        self.send_to_arduino('0')
                else:
                    # Manual mode - no detection
                    self.human_detected = False
                    self.detection_confidence = 0.0
                
                # Read Arduino status
                arduino_status = self.read_arduino_status()
                
                # Send status to dashboard every 0.5 seconds
                current_time = time.time()
                if current_time - last_status_send >= 0.5:
                    status_data = {
                        'humanDetected': self.human_detected,
                        'confidence': self.detection_confidence,
                        'mode': self.current_mode,
                        'timestamp': int(time.time() * 1000)
                    }
                    self.send_to_dashboard(status_data)
                    last_status_send = current_time
                
                # Display frame with detection info (optional, for debugging)
                if self.config.get('show_preview', False):
                    cv2.putText(
                        frame,
                        f"Mode: {self.current_mode} | Detected: {self.human_detected}",
                        (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.7,
                        (0, 255, 0) if self.human_detected else (0, 0, 255),
                        2
                    )
                    cv2.imshow('Vision Detector', frame)
                    if cv2.waitKey(1) & 0xFF == ord('q'):
                        break
                
                frame_count += 1
                time.sleep(1.0 / self.config['fps'])
        
        except KeyboardInterrupt:
            logger.info("Stopping vision detector...")
        except Exception as e:
            logger.error(f"Error in detection loop: {e}")
        finally:
            self.cleanup()
    
    def cleanup(self):
        """Clean up resources."""
        self.running = False
        if self.camera:
            self.camera.release()
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
        cv2.destroyAllWindows()
        logger.info("Vision detector stopped and cleaned up")


def main():
    """Main entry point."""
    try:
        detector = VisionDetector()
        detector.run()
    except Exception as e:
        logger.error(f"Fatal error: {e}")
        return 1
    return 0


if __name__ == '__main__':
    exit(main())

