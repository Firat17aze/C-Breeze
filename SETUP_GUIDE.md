# Setup Guide - IoT Smart Fan Control System

This guide will walk you through setting up the complete system step by step.

## Prerequisites

Before starting, ensure you have:

- [ ] Arduino Uno board
- [ ] HC-SR04 Ultrasonic Sensor
- [ ] L293D Motor Driver IC
- [ ] DC Fan (5V or 12V)
- [ ] USB cable for Arduino
- [ ] Jumper wires and breadboard
- [ ] External power supply (if using 12V fan)
- [ ] Laptop with webcam
- [ ] Internet connection (for downloading dependencies)

## Step 1: Hardware Assembly

### 1.1 Prepare Components

1. Place Arduino Uno on breadboard (optional, or use jumper wires)
2. Place L293D IC on breadboard (ensure correct orientation - notch on left)
3. Place HC-SR04 sensor on breadboard
4. Prepare DC fan with accessible terminals

### 1.2 Wire Arduino to L293D

```
Arduino Pin 3 (PD3)  → L293D Pin 7  (Input 2)
Arduino Pin 4 (PD4)  → L293D Pin 2  (Input 1)
Arduino Pin 5 (PD5)  → L293D Pin 1  (Enable 1)
Arduino 5V            → L293D Pin 16 (VCC Logic)
Arduino GND           → L293D Pin 4,5,12,13 (GND)
```

### 1.3 Wire Arduino to HC-SR04

```
Arduino Pin 9 (PB1)  → HC-SR04 Trig
Arduino Pin 10 (PB2) → HC-SR04 Echo
Arduino 5V            → HC-SR04 VCC
Arduino GND           → HC-SR04 GND
```

### 1.4 Wire L293D to Fan

```
L293D Pin 3 (Output 1) → Fan Terminal 1
L293D Pin 6 (Output 2) → Fan Terminal 2
```

### 1.5 Power Connections

- Connect USB cable to Arduino and laptop
- If using 12V fan: Connect external 12V supply to L293D Pin 8 (VCC2)
- Connect external power supply GND to Arduino GND

### 1.6 Verify Connections

- Double-check all connections
- Ensure no loose wires
- Verify power supply voltage matches fan requirements

## Step 2: Software Installation

### 2.1 Install Python (if not installed)

**Windows:**
1. Download Python 3.8+ from https://www.python.org/downloads/
2. Run installer, check "Add Python to PATH"
3. Verify: Open Command Prompt, type `python --version`

**Linux:**
```bash
sudo apt-get update
sudo apt-get install python3 python3-pip
```

**macOS:**
```bash
brew install python3
```

### 2.2 Install Node.js (if not installed)

**Windows:**
1. Download Node.js 16+ from https://nodejs.org/
2. Run installer
3. Verify: Open Command Prompt, type `node --version`

**Linux:**
```bash
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs
```

**macOS:**
```bash
brew install node
```

### 2.3 Install AVR Toolchain (for Arduino)

**Windows:**
1. Download WinAVR from http://winavr.sourceforge.net/
2. Or use MSYS2: https://www.msys2.org/
3. Install AVR-GCC, avr-libc, and avrdude

**Linux:**
```bash
sudo apt-get install gcc-avr avr-libc avr-binutils avrdude
```

**macOS:**
```bash
brew install avr-gcc avr-libc avrdude
```

Verify installation:
```bash
avr-gcc --version
avrdude -v
```

## Step 3: Configure and Upload Arduino Code

### 3.1 Find Your Serial Port

**Windows:**
1. Connect Arduino via USB
2. Open Device Manager
3. Look under "Ports (COM & LPT)"
4. Note the COM port number (e.g., COM3)

**Linux:**
```bash
ls /dev/ttyUSB* /dev/ttyACM*
# Usually /dev/ttyUSB0 or /dev/ttyACM0
```

**macOS:**
```bash
ls /dev/tty.usb*
# Usually /dev/tty.usbserial-* or /dev/tty.usbmodem*
```

### 3.2 Configure Makefile

Edit `arduino/Makefile`:

```makefile
PROGRAMMER = arduino
PORT = COM3        # Change to your port
BAUDRATE = 115200
```

### 3.3 Compile Arduino Code

```bash
cd arduino
make
```

Expected output:
```
avr-gcc -Wall -Os -mmcu=atmega328p -DF_CPU=16000000UL -std=c99 -c main.c -o main.o
avr-gcc -Wall -Os -mmcu=atmega328p -DF_CPU=16000000UL -std=c99 -c uart.c -o uart.o
avr-gcc -Wall -Os -mmcu=atmega328p -DF_CPU=16000000UL -std=c99 -c ultrasonic.c -o ultrasonic.o
avr-gcc -mmcu=atmega328p -o smart_fan.elf main.o uart.o ultrasonic.o
avr-objcopy -O ihex -R .eeprom smart_fan.elf smart_fan.hex
```

### 3.4 Upload to Arduino

```bash
make upload
```

**Alternative (if make upload doesn't work):**

Use Arduino IDE:
1. Open Arduino IDE
2. Tools → Board → Arduino Uno
3. Tools → Port → Select your COM port
4. Tools → Programmer → Arduino as ISP (or your programmer)
5. Sketch → Upload Using Programmer

Or use avrdude directly:
```bash
avrdude -c arduino -p atmega328p -P COM3 -b 115200 -U flash:w:smart_fan.hex:i
```

### 3.5 Verify Upload

1. Open Serial Monitor (Arduino IDE or any serial terminal)
2. Set baud rate to 9600
3. You should see: `SYSTEM:READY` and `MODE:AUTO`

## Step 4: Setup Python Vision Module

### 4.1 Install Python Dependencies

```bash
pip install -r requirements.txt
```

Or use virtual environment (recommended):
```bash
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install -r requirements.txt
```

### 4.2 Configure Vision Module

Edit `vision_config.json`:

```json
{
  "serial_port": "COM3",  // Change to your port
  "baud_rate": 9600,
  "camera_index": 0,
  "close_range_face_size": 100
}
```

### 4.3 Test Camera

```bash
python -c "import cv2; cap = cv2.VideoCapture(0); print('Camera OK' if cap.isOpened() else 'Camera Error'); cap.release()"
```

### 4.4 Test Vision Module (Optional)

Run with preview enabled:
1. Edit `vision_config.json`: Set `"show_preview": true`
2. Run: `python vision_detector.py`
3. You should see camera feed with detection overlay
4. Press 'q' to quit

## Step 5: Setup Backend Server

### 5.1 Install Backend Dependencies

```bash
cd backend
npm install
```

### 5.2 Configure Backend

Edit `backend/server.js`, update CONFIG:

```javascript
const CONFIG = {
    port: 3001,
    serialPort: 'COM3',  // Change to your port
    baudRate: 9600
};
```

Or use environment variable:
```bash
# Windows
set SERIAL_PORT=COM3
npm start

# Linux/macOS
SERIAL_PORT=/dev/ttyUSB0 npm start
```

### 5.3 Test Backend

```bash
npm start
```

Expected output:
```
Server running on http://localhost:3001
Serial port: COM3 at 9600 baud
Serial port opened: COM3
```

Test API:
```bash
curl http://localhost:3001/api/status
```

## Step 6: Setup Frontend Dashboard

### 6.1 Configure Frontend (if needed)

Edit `frontend/index.html` if backend is not on localhost:3001. Find these lines in the `<script>` section:

```javascript
const API_URL = 'http://localhost:3001';
const SOCKET_URL = 'http://localhost:3001';
```

Change to your backend URL if different.

### 6.2 Serve Frontend

**Option 1: Simple HTTP Server (Recommended)**

Using Python:
```bash
cd frontend
python -m http.server 8000
```

Using Node.js:
```bash
cd frontend
npx http-server -p 8000
```

Then open `http://localhost:8000` in your browser.

**Option 2: Open Directly**

Simply open `frontend/index.html` in your browser (may have CORS limitations).

## Step 7: System Integration Test

### 7.1 Start All Components

**Terminal 1 - Backend:**
```bash
cd backend
npm start
```

**Terminal 2 - Frontend (HTTP Server):**
```bash
cd frontend
python -m http.server 8000
# Or: npx http-server -p 8000
```

**Terminal 3 - Vision Module:**
```bash
python vision_detector.py
```

### 7.2 Test AUTO Mode

1. Open dashboard in browser: `http://localhost:8000` (or the port you used)
2. Ensure mode is set to AUTO
3. Stand in front of camera (within close range)
4. Move closer to ultrasonic sensor (< 50cm)
5. Fan should turn ON automatically
6. Move away (> 55cm)
7. Fan should turn OFF

### 7.3 Test MANUAL Mode

1. Click "MANUAL" button in dashboard
2. Click "Turn ON" button
3. Fan should turn ON
4. Click "Turn OFF" button
5. Fan should turn OFF

### 7.4 Verify Real-time Updates

- Distance should update in real-time
- Fan status should update immediately
- Detection status should reflect camera input
- Connection status should show green dot

## Step 8: Troubleshooting

### Arduino Not Detected
- Check USB cable connection
- Try different USB port
- Install Arduino drivers
- Check Device Manager (Windows)

### Serial Port Already in Use
- Close Arduino IDE Serial Monitor
- Close other programs using the port
- Restart backend server

### Camera Not Working
- Check camera permissions
- Try different camera_index (0, 1, 2...)
- Test with: `python -c "import cv2; print(cv2.VideoCapture(0).isOpened())"`

### Fan Not Spinning
- Check motor connections
- Verify power supply voltage
- Check L293D enable pin
- Test motor directly with battery

### Dashboard Not Connecting
- Check backend is running
- Verify WebSocket URL
- Check browser console for errors
- Verify CORS settings

## Step 9: Production Deployment

### 9.1 Deploy Frontend

The frontend is a single HTML file that can be deployed to any web server:

- Copy `frontend/index.html` to your web server
- Or serve it using any static file server (nginx, Apache, etc.)

### 9.2 Run Backend as Service

**Using PM2:**
```bash
npm install -g pm2
cd backend
pm2 start server.js
pm2 save
pm2 startup
```

**Using systemd (Linux):**
Create `/etc/systemd/system/smart-fan.service`:
```ini
[Unit]
Description=Smart Fan Backend
After=network.target

[Service]
Type=simple
User=your-user
WorkingDirectory=/path/to/backend
ExecStart=/usr/bin/node server.js
Restart=always

[Install]
WantedBy=multi-user.target
```

### 9.3 Run Vision Module as Service

**Using systemd (Linux):**
```ini
[Unit]
Description=Smart Fan Vision Module
After=network.target

[Service]
Type=simple
User=your-user
WorkingDirectory=/path/to/project
ExecStart=/usr/bin/python3 vision_detector.py
Restart=always

[Install]
WantedBy=multi-user.target
```

## Next Steps

- Calibrate distance threshold for your environment
- Adjust face detection sensitivity
- Add logging and monitoring
- Implement additional safety features
- Customize dashboard appearance

## Support

If you encounter issues:
1. Check the Troubleshooting section in README.md
2. Verify all connections
3. Review error messages in console/logs
4. Test each component individually

