# Quick Connection Reference

## 🎯 Where Each Wire Goes - Simple Guide

### Arduino Uno → Everything Else

```
ARDUINO UNO
┌─────────────────────┐
│                     │
│  Pin 3  ────────────┼──→ L293D Pin 7
│  Pin 4  ────────────┼──→ L293D Pin 2
│  Pin 5  ────────────┼──→ L293D Pin 1
│  Pin 9  ────────────┼──→ HC-SR04 Trig
│  Pin 10 ────────────┼──→ HC-SR04 Echo
│  5V     ────────────┼──→ L293D Pin 16
│  5V     ────────────┼──→ HC-SR04 VCC
│  GND    ────────────┼──→ L293D GND (pins 4,5,12,13)
│  GND    ────────────┼──→ HC-SR04 GND
│                     │
│  USB ───────────────┼──→ Laptop (for power & communication)
└─────────────────────┘
```

### L293D Motor Driver → Fan & Power

```
L293D MOTOR DRIVER (16-pin IC)
┌─────────────────────┐
│                     │
│  Pin 1  ←───────────┼─── Arduino Pin 5
│  Pin 2  ←───────────┼─── Arduino Pin 4
│  Pin 3  ────────────┼──→ Fan Wire 1
│  Pin 6  ───────────┼──→ Fan Wire 2
│  Pin 7  ←───────────┼─── Arduino Pin 3
│  Pin 8  ←───────────┼─── 12V Power Supply + (or 5V)
│  Pin 16 ←───────────┼─── Arduino 5V
│  GND    ←───────────┼─── Arduino GND
│                     │
└─────────────────────┘
```

### HC-SR04 Sensor

```
HC-SR04 ULTRASONIC SENSOR
┌─────────────────────┐
│                     │
│  VCC  ←─────────────┼─── Arduino 5V
│  GND  ←─────────────┼─── Arduino GND
│  Trig ←─────────────┼─── Arduino Pin 9
│  Echo ←─────────────┼─── Arduino Pin 10
│                     │
└─────────────────────┘
```

---

## 📋 Connection Checklist

### Step 1: HC-SR04 Sensor (4 wires)
- [ ] VCC → Arduino 5V
- [ ] GND → Arduino GND
- [ ] Trig → Arduino Pin 9
- [ ] Echo → Arduino Pin 10

### Step 2: L293D Motor Driver (8+ wires)
- [ ] Pin 1 → Arduino Pin 5
- [ ] Pin 2 → Arduino Pin 4
- [ ] Pin 3 → Fan Terminal 1
- [ ] Pin 6 → Fan Terminal 2
- [ ] Pin 7 → Arduino Pin 3
- [ ] Pin 8 → 12V Power Supply + (or 5V)
- [ ] Pin 16 → Arduino 5V
- [ ] Pins 4,5,12,13 → Arduino GND (all together)

### Step 3: DC Fan (2 wires)
- [ ] Terminal 1 → L293D Pin 3
- [ ] Terminal 2 → L293D Pin 6

### Step 4: Power
- [ ] USB cable → Arduino → Laptop
- [ ] 12V Power Supply + → L293D Pin 8 (if using 12V fan)
- [ ] 12V Power Supply - → Arduino GND (common ground)

---

## 🔌 Physical Connection Order

**Recommended order to avoid mistakes:**

1. **First: Power connections**
   - Connect USB to Arduino
   - Connect all GND wires together (Arduino GND, L293D GND, HC-SR04 GND)

2. **Second: Sensor (HC-SR04)**
   - VCC → 5V
   - GND → GND
   - Trig → Pin 9
   - Echo → Pin 10

3. **Third: Motor Driver (L293D)**
   - Logic power: Pin 16 → 5V
   - Control pins: Pin 1 → Pin 5, Pin 2 → Pin 4, Pin 7 → Pin 3
   - GND: All GND pins → Arduino GND

4. **Fourth: Fan**
   - Wire 1 → L293D Pin 3
   - Wire 2 → L293D Pin 6

5. **Last: Motor Power**
   - 12V + → L293D Pin 8 (if using 12V fan)
   - 12V - → Common GND

---

## ⚠️ Important Notes

1. **All GND must be connected together!**
   - Arduino GND
   - L293D GND (4 pins)
   - HC-SR04 GND
   - External power supply GND (if used)

2. **L293D Orientation:**
   - Notch or dot on the left side
   - Pin 1 is top-left when notch is on left

3. **Fan Direction:**
   - If fan spins wrong way, just swap the two fan wires

4. **Power Supply:**
   - 5V fan: Use Arduino 5V for L293D Pin 8
   - 12V fan: Use external 12V supply for L293D Pin 8

---

## 🧪 Quick Test

After connecting everything:

1. **Power Test:**
   - Arduino LED should light up
   - If not, check USB connection

2. **Sensor Test:**
   - Upload code and check Serial Monitor
   - Wave hand in front of sensor
   - Distance should change

3. **Fan Test:**
   - Upload code that turns fan on
   - Fan should spin
   - If not, check power and connections

---

## 📸 Visual Guide

If you're using a breadboard, here's the layout:

```
Breadboard Layout:
┌─────────────────────────────────────┐
│  +5V Rail  │  GND Rail              │
├─────────────────────────────────────┤
│                                     │
│  [L293D IC here - notch left]      │
│  Pin 1  → Arduino Pin 5            │
│  Pin 2  → Arduino Pin 4            │
│  Pin 3  → Fan Wire 1               │
│  Pin 6  → Fan Wire 2               │
│  Pin 7  → Arduino Pin 3            │
│  Pin 8  → 12V Power +              │
│  Pin 16 → +5V Rail                  │
│  GND    → GND Rail                  │
│                                     │
│  [HC-SR04 Sensor]                  │
│  VCC  → +5V Rail                    │
│  GND  → GND Rail                    │
│  Trig → Arduino Pin 9               │
│  Echo → Arduino Pin 10              │
│                                     │
└─────────────────────────────────────┘
```

---

## 🆘 Still Confused?

1. **Start with just Arduino + USB** - Make sure Arduino works
2. **Add sensor next** - Test sensor separately
3. **Add motor driver** - Test fan control separately
4. **Connect everything** - Test full system

Take it one step at a time! 🚀

