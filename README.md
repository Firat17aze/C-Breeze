# IoT Smart Fan Control System

A complete smart fan control system that combines computer vision (laptop camera), Arduino Uno hardware control, and a real-time web dashboard. The system operates in two modes: automatic (sensor-driven) and manual (dashboard-controlled).

## System Architecture

```
┌─────────────────┐         ┌──────────────┐         ┌─────────────┐
│  Python Vision  │────────▶│   Arduino    │◀────────│   Node.js   │
│     Module      │  Serial │     Uno      │  Serial │   Backend   │
│   (OpenCV)      │         │  (Bare C)    │         │  (Express)  │
└─────────────────┘         └──────────────┘         └──────┬──────┘
                                                             │
                                                             │ WebSocket
                                                             │
                                                      ┌──────▼──────┐
                                                      │   React     │
                                                      │  Frontend   │
                                                      │  Dashboard  │
                                                      └─────────────┘
```

## Features

- **Automatic Mode**: Camera detects humans in close range, triggers ultrasonic sensor, and automatically controls fan
- **Manual Mode**: Direct fan control via web dashboard
- **Real-time Updates**: Live status updates via WebSocket
- **Distance Monitoring**: Ultrasonic sensor measures proximity (0-400cm)
- **Safety Features**: Fan timeout, error handling, graceful degradation

## Hardware Requirements

- Arduino Uno (ATmega328P)
- HC-SR04 Ultrasonic Sensor
- L293D Motor Driver IC (Dual H-Bridge)
- DC Fan (12V or 5V depending on motor driver setup)
- USB cable for Arduino-Laptop connection
- External power supply for motor (if using 12V fan)
- Jumper wires and breadboard
- Laptop with webcam

## Pin Configuration

### Arduino Uno Pin Connections

```
Arduino Pin 4 (PD4)  → L293D Input 1 (Forward Logic)
Arduino Pin 3 (PD3)  → L293D Input 2 (Backward Logic)
Arduino Pin 5 (PD5)  → L293D Enable (Speed/On-Off Switch)
Arduino Pin 9 (PB1)  → HC-SR04 Trigger
Arduino Pin 10 (PB2) → HC-SR04 Echo
```

### L293D Motor Driver Connections

```
L293D Pin 1  → Enable (Arduino Pin 5)
L293D Pin 2  → Input 1 (Arduino Pin 4)
L293D Pin 7  → Input 2 (Arduino Pin 3)
L293D Pin 3  → Motor Terminal 1
L293D Pin 6  → Motor Terminal 2
L293D Pin 8  → Motor Power Supply (VCC2)
L293D Pin 16 → Logic Power (5V)
L293D Pin 4,5,12,13 → Ground
```

### HC-SR04 Ultrasonic Sensor Connections

```
HC-SR04 VCC  → 5V
HC-SR04 GND  → Ground
HC-SR04 Trig → Arduino Pin 9 (PB1)
HC-SR04 Echo → Arduino Pin 10 (PB2)
```

## Software Requirements

- Python 3.8+
- Node.js 16+
- AVR-GCC toolchain (for Arduino compilation)
- Arduino IDE or avrdude (for uploading)

## Quick Start

### Hardware Connections
**New to hardware?** Start here:
1. See **[CONNECTION_GUIDE.md](CONNECTION_GUIDE.md)** for detailed step-by-step wiring instructions
2. See **[QUICK_CONNECT.md](QUICK_CONNECT.md)** for a quick reference card

### Software Setup (VS Code / Development)
**Want to run everything in VS Code?** See:
1. **[SOFTWARE_SETUP.md](SOFTWARE_SETUP.md)** - Complete guide for running in VS Code
2. Use `start-all.bat` (Windows) or `start-all.sh` (Linux/Mac) to start everything at once
3. Or use VS Code Tasks: Press `Ctrl+Shift+P` → "Tasks: Run Task" → "Start All Services"

**Quick Summary:**
- Arduino Pin 3, 4, 5 → L293D Motor Driver
- Arduino Pin 9, 10 → HC-SR04 Ultrasonic Sensor
- L293D → DC Fan
- All GND connected together

## Installation

### 1. Python Vision Module

```bash
# Install Python dependencies
pip install -r requirements.txt

# Configure serial port in vision_config.json
# Edit "serial_port" to match your system (COM3 on Windows, /dev/ttyUSB0 on Linux)
```

### 2. Arduino Code

```bash
cd arduino

# Compile the code
make

# Upload to Arduino (adjust PORT in Makefile)
make upload
```

**Note**: Make sure you have AVR-GCC installed:
- **Windows**: Install [WinAVR](http://winavr.sourceforge.net/) or use [MSYS2](https://www.msys2.org/)
- **Linux**: `sudo apt-get install gcc-avr avr-libc avrdude`
- **macOS**: `brew install avr-gcc avr-libc avrdude`

### 3. Backend Server

```bash
cd backend

# Install dependencies
npm install

# Configure serial port in server.js or use environment variable
# SERIAL_PORT=COM3 node server.js

# Start server
npm start
```

### 4. Frontend Dashboard

The frontend is a single HTML file that can be opened directly in a browser or served via a web server.

**Option 1: Open directly in browser**
- Simply open `frontend/index.html` in your web browser
- Note: You may need to serve it via HTTP due to CORS restrictions

**Option 2: Serve with a simple HTTP server**
```bash
cd frontend

# Using Python 3
python -m http.server 8000

# Using Node.js (if you have http-server installed)
npx http-server -p 8000

# Using PHP
php -S localhost:8000
```

Then open `http://localhost:8000` in your browser.

## Usage

### Starting the System

1. **Upload Arduino Code**: Compile and upload the bare metal C code to Arduino
2. **Start Backend Server**: Run `npm start` in the `backend` directory
3. **Start Frontend**: Serve `frontend/index.html` using a web server (e.g., `python -m http.server 8000` in the frontend directory)
4. **Start Face Tracking**: Run `python app.py`

### Operating Modes

#### AUTO Mode (Default)
- Camera continuously monitors for humans
- When human detected in close range → Signal sent to Arduino
- Arduino activates ultrasonic sensor
- If distance ≤ 50cm AND human detected → Fan ON
- If distance > 55cm OR no human → Fan OFF
- Dashboard displays real-time status

#### MANUAL Mode
- Camera detection DISABLED
- Sensor readings DISABLED (but still displayed)
- Dashboard button controls fan directly
- User clicks "Turn Fan ON/OFF" → Command sent to Arduino
- Arduino responds to manual commands only

## Communication Protocol

### Laptop ↔ Arduino (Serial/UART)

**Laptop → Arduino:**
- `'A'` or `'1'` or `'H'`: Activate sensor (human detected in close range)
- `'D'` or `'0'` or `'L'`: Deactivate sensor (no human or out of range)
- `'M'`: Switch to manual mode
- `'O'`: Switch to auto mode
- `'F1'`: Turn fan ON (manual mode only)
- `'F0'`: Turn fan OFF (manual mode only)

**Arduino → Laptop:**
- `"DIST:XXX\n"`: Distance in cm
- `"FAN:ON\n"` or `"FAN:OFF\n"`: Fan status
- `"MODE:AUTO\n"` or `"MODE:MANUAL\n"`: Current mode

### Backend ↔ Frontend (WebSocket)

```json
{
  "distance": 45,
  "fanStatus": "ON",
  "humanDetected": true,
  "mode": "AUTO",
  "timestamp": 1638360000000
}
```

## Configuration

### Vision Module (`vision_config.json`)

```json
{
  "serial_port": "COM3",
  "baud_rate": 9600,
  "camera_index": 0,
  "close_range_face_size": 100,
  "frame_width": 640,
  "frame_height": 480,
  "fps": 30
}
```

### Backend (`server.js`)

Edit the `CONFIG` object:
```javascript
const CONFIG = {
    port: 3001,
    serialPort: 'COM3',  // Change to /dev/ttyUSB0 on Linux
    baudRate: 9600,
    distanceThreshold: 50,
    hysteresis: 5
};
```

### Arduino (`main.c`)

Edit these constants:
```c
#define DISTANCE_THRESHOLD_CM 50
#define HYSTERESIS_CM 5
```

## Troubleshooting

### Camera Not Detecting
- Check camera permissions
- Verify camera index in `vision_config.json`
- Test camera with: `python -c "import cv2; cap = cv2.VideoCapture(0); print(cap.isOpened())"`

### Serial Port Issues
- **Windows**: Check Device Manager for COM port number
- **Linux**: Check `/dev/ttyUSB*` or `/dev/ttyACM*`
- Ensure only one program accesses the serial port at a time
- Close Arduino IDE Serial Monitor if open

### Arduino Not Responding
- Verify code uploaded successfully
- Check wiring connections
- Test serial communication with a simple echo program
- Verify baud rate matches (9600)

### Ultrasonic Sensor Issues
- Ensure sensor is powered (5V)
- Check trigger and echo connections
- Verify sensor is not too close to obstacles
- Test sensor with Arduino Serial Monitor

### Web Dashboard Not Updating
- Check browser console for errors
- Verify backend server is running
- Check WebSocket connection status (green dot in dashboard)
- Verify API URL in frontend code

## Safety Features

- **Fan Timeout**: Automatically turns off after 10 minutes of continuous operation
- **Error Handling**: Graceful handling of sensor disconnection
- **Serial Timeout**: Prevents hanging on communication errors
- **Camera Fallback**: System continues operating even if camera fails

## Project Structure

```
pitchpredict-app/
├── vision_detector.py          # Python vision module
├── vision_config.json          # Vision module configuration
├── requirements.txt            # Python dependencies
├── arduino/
│   ├── main.c                  # Main Arduino program
│   ├── uart.h/c                # UART communication module
│   ├── ultrasonic.h/c          # Ultrasonic sensor driver
│   └── Makefile                # Build configuration
├── backend/
│   ├── server.js               # Node.js backend server
│   └── package.json            # Backend dependencies
├── frontend/
│   └── index.html              # Single-file dashboard (HTML + CSS + JS)
└── README.md                   # This file
```

## Development

### Testing Individual Components

1. **Test Arduino Serial**: Use Arduino Serial Monitor at 9600 baud
2. **Test Vision Module**: Run with `show_preview: true` in config
3. **Test Backend**: Use `curl http://localhost:3001/api/status`
4. **Test Frontend**: Check browser console and network tab

### Building for Production

```bash
# Frontend
cd frontend
npm run build

# Backend (use PM2 or similar)
npm start
```

## License

MIT License - Feel free to use and modify for your projects.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Support

For issues or questions:
1. Check the Troubleshooting section
2. Review the code comments
3. Check serial port and hardware connections
4. Verify all dependencies are installed correctly

