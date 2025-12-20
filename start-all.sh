#!/bin/bash

echo "========================================"
echo "Starting Smart Fan Control System"
echo "========================================"
echo ""

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv venv
    echo "Installing Python dependencies..."
    source venv/bin/activate
    pip install -r requirements.txt
    deactivate
fi

echo "[1/3] Starting Backend Server..."
cd backend
npm start &
BACKEND_PID=$!
cd ..
sleep 3

echo "[2/3] Starting Python Face Tracking (app.py)..."
source venv/bin/activate
python app.py &
VISION_PID=$!
deactivate
sleep 2

echo "[3/3] Starting Frontend Server..."
cd frontend
python3 -m http.server 8000 &
FRONTEND_PID=$!
cd ..
sleep 2

echo ""
echo "========================================"
echo "All services started!"
echo "========================================"
echo "Backend:    http://localhost:3001"
echo "Frontend:   http://localhost:8000"
echo ""
echo "Opening dashboard in browser..."
sleep 2

# Open browser (Linux)
if command -v xdg-open &> /dev/null; then
    xdg-open http://localhost:8000
# macOS
elif command -v open &> /dev/null; then
    open http://localhost:8000
fi

echo ""
echo "Services are running in background."
echo "Press Ctrl+C to stop all services..."
echo ""

# Wait for interrupt
trap "kill $BACKEND_PID $VISION_PID $FRONTEND_PID 2>/dev/null; exit" INT
wait

