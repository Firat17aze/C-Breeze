@echo off
echo ========================================
echo Starting Smart Fan Control System
echo ========================================
echo.

echo [1/3] Starting Backend Server...
start "Backend Server" cmd /k "cd backend && npm start"
timeout /t 3 /nobreak >nul

echo [2/3] Starting Python Face Tracking (app.py)...
start "Python Face Tracking" cmd /k "cd /d %~dp0 && if exist venv\Scripts\activate.bat (call venv\Scripts\activate.bat && python app.py) else (echo Virtual environment not found! Please run: python -m venv venv && venv\Scripts\activate && pip install -r requirements.txt)"
timeout /t 2 /nobreak >nul

echo [3/3] Starting Frontend Server...
start "Frontend Server" cmd /k "cd frontend && python -m http.server 8000"
timeout /t 2 /nobreak >nul

echo.
echo ========================================
echo All services starting!
echo ========================================
echo Backend:    http://localhost:3001
echo Frontend:   http://localhost:8000
echo.
echo Opening dashboard in browser...
timeout /t 2 /nobreak >nul
start http://localhost:8000

echo.
echo Press any key to exit this window (services will keep running)...
pause >nul

