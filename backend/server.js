/**
 * IoT Smart Fan Control System - Backend Server
 * Node.js + Express + Socket.io + Serial Port
 */

const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const SerialPort = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const cors = require('cors');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
    cors: {
        origin: "*",
        methods: ["GET", "POST"]
    }
});

// Configuration
const CONFIG = {
    port: 3001,
    serialPort: process.env.SERIAL_PORT || 'COM5',  // Change to /dev/ttyUSB0 on Linux
    baudRate: 9600,
    distanceThreshold: 50,
    hysteresis: 5
};

// State management
let systemState = {
    distance: 0,
    fanStatus: 'OFF',
    humanDetected: false,
    mode: 'AUTO',
    timestamp: Date.now()
};

// Serial port connection
let serialPort = null;
let parser = null;

/**
 * Initialize serial port connection to Arduino
 */
function initSerialPort() {
    try {
        serialPort = new SerialPort.SerialPort({
            path: CONFIG.serialPort,
            baudRate: CONFIG.baudRate,
            autoOpen: false
        });

        parser = serialPort.pipe(new ReadlineParser({ delimiter: '\n' }));

        serialPort.open((err) => {
            if (err) {
                console.error('Error opening serial port:', err.message);
                console.error(`Please check if Arduino is connected to ${CONFIG.serialPort}`);
                return;
            }
            console.log(`Serial port opened: ${CONFIG.serialPort}`);
        });

        // Handle incoming data from Arduino
        parser.on('data', (data) => {
            const line = data.toString().trim();
            console.log('Arduino:', line);
            parseArduinoMessage(line);
        });

        // Handle serial port errors
        serialPort.on('error', (err) => {
            console.error('Serial port error:', err.message);
        });

        serialPort.on('close', () => {
            console.log('Serial port closed');
        });

    } catch (error) {
        console.error('Failed to initialize serial port:', error);
    }
}

/**
 * Parse messages from Arduino
 */
function parseArduinoMessage(message) {
    if (message.startsWith('DIST:')) {
        const distance = parseInt(message.substring(5));
        if (!isNaN(distance) && distance > 0) {
            // Only update if distance is valid (> 0)
            // Ignore 0 readings which are sensor errors
            systemState.distance = distance;
            systemState.timestamp = Date.now();
            broadcastState();
        }
    } else if (message.startsWith('FAN:')) {
        const status = message.substring(4).trim();
        if (status === 'ON' || status === 'OFF') {
            systemState.fanStatus = status;
            systemState.timestamp = Date.now();
            broadcastState();
        }
    } else if (message.startsWith('MODE:')) {
        // Ignore mode updates from Arduino - backend controls mode
        // This prevents Arduino from overriding user's mode selection
        console.log(`Arduino mode message ignored: ${message}`);
    } else if (message.startsWith('SYSTEM:')) {
        console.log('System message:', message);
    }
}

/**
 * Send command to Arduino
 */
function sendToArduino(command) {
    if (serialPort && serialPort.isOpen) {
        serialPort.write(command, (err) => {
            if (err) {
                console.error('Error writing to serial port:', err);
            } else {
                console.log('Sent to Arduino:', command);
            }
        });
    } else {
        console.warn('Serial port not open, cannot send command:', command);
    }
}

/**
 * Broadcast current state to all connected clients
 */
function broadcastState() {
    io.emit('stateUpdate', systemState);
}

// Middleware
app.use(cors());
app.use(express.json());

// API Routes

/**
 * GET /api/status - Get current system status
 */
app.get('/api/status', (req, res) => {
    res.json(systemState);
});

/**
 * POST /api/mode - Change operating mode
 */
app.post('/api/mode', (req, res) => {
    const { mode } = req.body;
    if (mode === 'AUTO' || mode === 'MANUAL') {
        systemState.mode = mode;
        systemState.timestamp = Date.now();
        
        // Send command to Arduino
        if (mode === 'MANUAL') {
            sendToArduino('M');
        } else {
            sendToArduino('O');
        }
        
        broadcastState();
        res.json({ success: true, mode: mode });
    } else {
        res.status(400).json({ success: false, error: 'Invalid mode. Use AUTO or MANUAL' });
    }
});

/**
 * POST /api/fan - Manual fan control
 */
app.post('/api/fan', (req, res) => {
    const { action } = req.body;
    
    console.log(`Manual fan control request: ${action}, Current mode: ${systemState.mode}`);
    
    if (systemState.mode !== 'MANUAL') {
        return res.status(400).json({ 
            success: false, 
            error: 'Fan can only be controlled manually in MANUAL mode' 
        });
    }
    
    if (action === 'ON') {
        sendToArduino('F1');
        systemState.fanStatus = 'ON';
        console.log('Manual control: Fan turned ON');
    } else if (action === 'OFF') {
        sendToArduino('F0');
        systemState.fanStatus = 'OFF';
        console.log('Manual control: Fan turned OFF');
    } else {
        return res.status(400).json({ 
            success: false, 
            error: 'Invalid action. Use ON or OFF' 
        });
    }
    
    systemState.timestamp = Date.now();
    broadcastState();
    res.json({ success: true, fanStatus: systemState.fanStatus });
});

/**
 * POST /api/vision-update - Receive updates from Python vision module
 */
app.post('/api/vision-update', (req, res) => {
    const { humanDetected, confidence, timestamp } = req.body;
    console.log(`Vision update received: humanDetected=${humanDetected}, confidence=${confidence}, mode=${systemState.mode}, distance=${systemState.distance}cm`);
    
    // Update detection status
    if (humanDetected !== undefined) {
        systemState.humanDetected = humanDetected;
        
        // In AUTO mode, control fan based on BOTH detection AND distance
        if (systemState.mode === 'AUTO') {
            // Only make decisions when we have valid distance reading
            if (systemState.distance > 0) {
                // Fan ON only if: human detected AND distance <= threshold
                const shouldTurnOn = humanDetected && 
                                   systemState.distance <= CONFIG.distanceThreshold;
                
                if (shouldTurnOn && systemState.fanStatus !== 'ON') {
                    sendToArduino('F1');
                    systemState.fanStatus = 'ON';
                    console.log(`AUTO: Fan ON - Human: ${humanDetected}, Distance: ${systemState.distance}cm`);
                } else if (!shouldTurnOn && systemState.fanStatus !== 'OFF') {
                    sendToArduino('F0');
                    systemState.fanStatus = 'OFF';
                    if (humanDetected && systemState.distance > CONFIG.distanceThreshold) {
                        console.log(`AUTO: Fan OFF - Human detected but too far: ${systemState.distance}cm`);
                    } else {
                        console.log(`AUTO: Fan OFF - No human detected`);
                    }
                }
            }
            // If distance is 0 (sensor error), keep current fan state
        }
    }
    
    systemState.timestamp = timestamp || Date.now();
    
    broadcastState();
    res.json({ success: true });
});

/**
 * POST /api/config - Update configuration
 */
app.post('/api/config', (req, res) => {
    const { distanceThreshold, hysteresis } = req.body;
    
    if (distanceThreshold !== undefined) {
        CONFIG.distanceThreshold = distanceThreshold;
    }
    if (hysteresis !== undefined) {
        CONFIG.hysteresis = hysteresis;
    }
    
    res.json({ success: true, config: CONFIG });
});

// WebSocket connection handling
io.on('connection', (socket) => {
    console.log('Client connected:', socket.id);
    
    // Send current state to newly connected client
    socket.emit('stateUpdate', systemState);
    
    // Handle mode change from client
    socket.on('changeMode', (data) => {
        const { mode } = data;
        if (mode === 'AUTO' || mode === 'MANUAL') {
            systemState.mode = mode;
            systemState.timestamp = Date.now();
            
            if (mode === 'MANUAL') {
                sendToArduino('M');
            } else {
                sendToArduino('O');
            }
            
            broadcastState();
        }
    });
    
    // Handle manual fan control from client
    socket.on('controlFan', (data) => {
        const { action } = data;
        
        if (systemState.mode === 'MANUAL') {
            if (action === 'ON') {
                sendToArduino('F1');
                systemState.fanStatus = 'ON';
            } else if (action === 'OFF') {
                sendToArduino('F0');
                systemState.fanStatus = 'OFF';
            }
            
            systemState.timestamp = Date.now();
            broadcastState();
        }
    });
    
    socket.on('disconnect', () => {
        console.log('Client disconnected:', socket.id);
    });
});

// Health check endpoint
app.get('/health', (req, res) => {
    res.json({ 
        status: 'ok', 
        serialPortOpen: serialPort ? serialPort.isOpen : false,
        timestamp: Date.now()
    });
});

// Start server
function startServer() {
    // Initialize serial port
    initSerialPort();
    
    // Start HTTP server
    server.listen(CONFIG.port, () => {
        console.log(`Server running on http://localhost:${CONFIG.port}`);
        console.log(`Serial port: ${CONFIG.serialPort} at ${CONFIG.baudRate} baud`);
    });
}

// Graceful shutdown
process.on('SIGINT', () => {
    console.log('\nShutting down server...');
    if (serialPort && serialPort.isOpen) {
        serialPort.close();
    }
    server.close(() => {
        console.log('Server closed');
        process.exit(0);
    });
});

// Start the server
startServer();

