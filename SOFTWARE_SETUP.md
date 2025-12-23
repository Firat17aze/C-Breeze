# Software Setup Guide - Running Everything in VS Code

This guide shows you how to set up and run all the software components (Python, Node.js backend, and frontend) in VS Code or other development environments.

## Prerequisites

Make sure you have installed:
- [ ] Python 3.8+ (with pip)
- [ ] Node.js 16+ (with npm)
- [ ] VS Code (or any code editor)

---

## Option 1: VS Code with Integrated Terminal (Recommended)

### Step 1: Open Project in VS Code

1. Open VS Code
2. File → Open Folder
3. Select your project folder (`pitchpredict-app`)

### Step 2: Open Multiple Terminals

VS Code allows you to run multiple terminals side-by-side:

1. **Open first terminal:**
   - Press `` Ctrl + ` `` (backtick) or View → Terminal
   - This opens Terminal 1

2. **Split terminal:**
   - Click the `+` button next to the terminal dropdown
   - Or press `` Ctrl + Shift + ` ``
   - This creates Terminal 2

3. **Split again for Terminal 3:**
   - Click `+` again or use the dropdown → "Split Terminal"

You should now have 3 terminals open.

### Step 3: Configure Each Terminal

**Terminal 1 - Backend Server:**
```bash
cd backend
npm install
npm start
```

**Terminal 2 - Python Face Tracking (app.py):**
```bash
# Create virtual environment (first time only)
python -m venv venv

# Activate virtual environment
# Windows:
venv\Scripts\activate
# Linux/Mac:
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Run face tracking
python app.py
```

**Terminal 3 - Frontend (HTTP Server):**
```bash
cd frontend
python -m http.server 8000
# Or: npx http-server -p 8000
```

---

## Option 2: VS Code Tasks (Automated)

Create a VS Code task configuration to run everything with one command.

### Create `.vscode/tasks.json`

Create a folder `.vscode` in your project root, then create `tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Start Backend Server",
            "type": "shell",
            "command": "npm start",
            "options": {
                "cwd": "${workspaceFolder}/backend"
            },
            "isBackground": true,
            "problemMatcher": {
                "pattern": {
                    "regexp": "^.*$",
                    "file": 1,
                    "location": 2,
                    "message": 3
                },
                "background": {
                    "activeOnStart": true,
                    "beginsPattern": ".*",
                    "endsPattern": "Server running on.*"
                }
            }
        },
        {
            "label": "Start Python Vision",
            "type": "shell",
            "command": "python app.py",
            "options": {
                "cwd": "${workspaceFolder}",
                "env": {
                    "PYTHONPATH": "${workspaceFolder}"
                }
            },
            "windows": {
                "command": "${workspaceFolder}\\venv\\Scripts\\python.exe app.py"
            },
            "linux": {
                "command": "${workspaceFolder}/venv/bin/python app.py"
            },
            "isBackground": true
        },
        {
            "label": "Start Frontend Server",
            "type": "shell",
            "command": "python -m http.server 8000",
            "options": {
                "cwd": "${workspaceFolder}/frontend"
            },
            "isBackground": true
        },
        {
            "label": "Start All Services",
            "dependsOn": [
                "Start Backend Server",
                "Start Python Vision",
                "Start Frontend Server"
            ],
            "problemMatcher": []
        }
    ]
}
```

### How to Use Tasks:

1. Press `Ctrl + Shift + P` (Command Palette)
2. Type "Tasks: Run Task"
3. Select "Start All Services"
4. All three services will start automatically!

---

## Option 3: Using npm Scripts (Node.js Way)

Create a root `package.json` to manage all services:

### Create `package.json` in project root:

```json
{
  "name": "smart-fan-system",
  "version": "1.0.0",
  "scripts": {
    "install-all": "cd backend && npm install && cd ../frontend && echo 'Frontend is static HTML, no install needed'",
    "backend": "cd backend && npm start",
    "frontend": "cd frontend && python -m http.server 8000",
    "vision": "python vision_detector.py",
    "start": "concurrently \"npm run backend\" \"npm run vision\" \"npm run frontend\"",
    "start:backend": "cd backend && npm start",
    "start:vision": "python vision_detector.py",
    "start:frontend": "cd frontend && python -m http.server 8000"
  },
  "devDependencies": {
    "concurrently": "^8.2.0"
  }
}
```

### Install concurrently (runs multiple commands):

```bash
npm install
```

### Run everything at once:

```bash
npm start
```

This will start all three services in one terminal!

---

## Option 4: Separate Terminal Windows (Simple)

If you prefer separate windows:

### Windows PowerShell/CMD:

**Window 1 - Backend:**
```powershell
cd C:\Users\camal\pitchpredict-app\backend
npm install
npm start
```

**Window 2 - Python:**
```powershell
cd C:\Users\camal\pitchpredict-app
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
python vision_detector.py
```

**Window 3 - Frontend:**
```powershell
cd C:\Users\camal\pitchpredict-app\frontend
python -m http.server 8000
```

### Linux/Mac Terminal:

**Terminal 1 - Backend:**
```bash
cd ~/pitchpredict-app/backend
npm install
npm start
```

**Terminal 2 - Python:**
```bash
cd ~/pitchpredict-app
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python vision_detector.py
```

**Terminal 3 - Frontend:**
```bash
cd ~/pitchpredict-app/frontend
python3 -m http.server 8000
```

---

## VS Code Extensions (Helpful)

Install these VS Code extensions for better development:

1. **Python** (by Microsoft)
   - Syntax highlighting, debugging, IntelliSense

2. **ES6 String HTML** (by Tobermory)
   - Better HTML editing

3. **Live Server** (by Ritwick Dey)
   - Alternative to `http-server` for frontend

4. **Terminal Tabs** (by Tyriar)
   - Better terminal management

5. **Task Explorer** (by actboy168)
   - Visual task runner

---

## Configuration Files Setup

### 1. Backend Configuration

Edit `backend/server.js` - Update the serial port:

```javascript
const CONFIG = {
    port: 3001,
    serialPort: 'COM3',  // Change to your Arduino port
    baudRate: 9600
};
```

Or use environment variable:
```bash
# Windows
set SERIAL_PORT=COM3
npm start

# Linux/Mac
export SERIAL_PORT=/dev/ttyUSB0
npm start
```

### 2. Python Vision Configuration

Edit `vision_config.json`:

```json
{
  "serial_port": "COM3",  // Change to your Arduino port
  "baud_rate": 9600,
  "camera_index": 0
}
```

### 3. Frontend Configuration

Edit `frontend/index.html` - Find this in the `<script>` section:

```javascript
const API_URL = 'http://localhost:3001';
const SOCKET_URL = 'http://localhost:3001';
```

Change if your backend runs on different port/host.

---

## Running Order

**Important:** Start services in this order:

1. **First:** Backend Server (must be running for others to connect)
2. **Second:** Python Vision Module (connects to backend)
3. **Third:** Frontend (opens in browser)

### Quick Start Scripts

**Windows - `start-all.bat`:**
```batch
@echo off
start "Backend" cmd /k "cd backend && npm start"
timeout /t 3
start "Python Vision" cmd /k "cd .. && venv\Scripts\activate && python vision_detector.py"
timeout /t 2
start "Frontend" cmd /k "cd frontend && python -m http.server 8000"
timeout /t 2
start http://localhost:8000
```

**Linux/Mac - `start-all.sh`:**
```bash
#!/bin/bash
gnome-terminal -- bash -c "cd backend && npm start; exec bash"
sleep 3
gnome-terminal -- bash -c "source venv/bin/activate && python vision_detector.py; exec bash"
sleep 2
gnome-terminal -- bash -c "cd frontend && python3 -m http.server 8000; exec bash"
sleep 2
xdg-open http://localhost:8000
```

Make executable:
```bash
chmod +x start-all.sh
```

---

## Debugging in VS Code

### Python Debugging

Create `.vscode/launch.json`:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python: Face Tracking (app.py)",
            "type": "python",
            "request": "launch",
            "program": "${workspaceFolder}/app.py",
            "console": "integratedTerminal",
            "justMyCode": true
        }
    ]
}
```

Press `F5` to debug Python code!

### Node.js Debugging

VS Code automatically detects Node.js. Just:
1. Set breakpoints in `backend/server.js`
2. Press `F5`
3. Select "Node.js" environment

---

## Checking if Everything is Running

### Backend (Port 3001):
- Open browser: `http://localhost:3001/api/status`
- Should return JSON with system status

### Frontend (Port 8000):
- Open browser: `http://localhost:8000`
- Should show dashboard

### Python Vision:
- Check terminal for "Camera initialized successfully"
- Check for "Serial connection established"

---

## Common Issues

### Port Already in Use
```bash
# Windows - Find what's using port 3001
netstat -ano | findstr :3001

# Linux/Mac
lsof -i :3001
```

### Python Module Not Found
```bash
# Make sure virtual environment is activated
# Windows:
venv\Scripts\activate

# Linux/Mac:
source venv/bin/activate

# Then install:
pip install -r requirements.txt
```

### Serial Port Access Denied
- Close Arduino IDE Serial Monitor
- Close other programs using the port
- Run VS Code as Administrator (Windows)

---

## Recommended VS Code Workspace

Create `smart-fan.code-workspace`:

```json
{
    "folders": [
        {
            "path": "."
        },
        {
            "path": "./backend"
        },
        {
            "path": "./frontend"
        },
        {
            "path": "./arduino"
        }
    ],
    "settings": {
        "python.defaultInterpreterPath": "${workspaceFolder}/venv/bin/python",
        "files.exclude": {
            "**/node_modules": true,
            "**/__pycache__": true,
            "**/*.pyc": true
        }
    }
}
```

Open this file in VS Code to get a multi-root workspace!

---

## Quick Commands Cheat Sheet

```bash
# Install everything
cd backend && npm install
cd .. && python -m venv venv && venv\Scripts\activate && pip install -r requirements.txt

# Start backend
cd backend && npm start

# Start Python
python vision_detector.py

# Start frontend
cd frontend && python -m http.server 8000

# Check if running
curl http://localhost:3001/api/status
```

---

That's it! You now have multiple ways to run everything in VS Code or any development environment! 🚀

