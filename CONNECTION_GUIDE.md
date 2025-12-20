# Hardware Connection Guide - Step by Step

This guide shows you exactly where to connect each wire on your Arduino Uno and components.

## Components Needed

- Arduino Uno
- HC-SR04 Ultrasonic Sensor
- L293D Motor Driver IC (16-pin DIP)
- DC Fan (5V or 12V)
- USB Cable (for Arduino)
- Jumper Wires (male-to-male)
- Breadboard (optional, but recommended)
- External 12V Power Supply (if using 12V fan)

---

## Step 1: Identify Arduino Uno Pins

First, let's identify the pins we'll use on the Arduino Uno:

```
                    ┌─────┬─────┐
                 ┌──│ 0   │ 13  │──┐
                 │  │ 1   │ 12  │  │
                 │  │ 2   │ 11  │  │
                 │  │ 3◄──│ 10◄─┤  │ ← We use these
                 │  │ 4◄──│  9◄─┤  │ ← We use these
                 │  │ 5◄──│  8  │  │ ← We use this
                 │  │ 6   │  7  │  │
                 └──┴─────┴─────┴──┘
                      │     │
                   GND   5V
```

**Pins we'll use:**
- **Pin 3** (PD3) - Digital pin
- **Pin 4** (PD4) - Digital pin
- **Pin 5** (PD5) - Digital pin
- **Pin 9** (PB1) - Digital pin
- **Pin 10** (PB2) - Digital pin
- **5V** - Power pin
- **GND** - Ground pin (there are multiple, use any)

---

## Step 2: Connect HC-SR04 Ultrasonic Sensor

The HC-SR04 has 4 pins. Here's how to connect them:

### HC-SR04 Pin Layout (from left to right when facing the sensor):
```
┌─────────────────┐
│  VCC  Trig  Echo │
│   │    │     │   │
│   │    │     │   │
│   GND  (not used)│
└─────────────────┘
```

### Connections:

1. **HC-SR04 VCC** → **Arduino 5V**
   - Red wire or any colored wire
   - Connect to the 5V pin on Arduino

2. **HC-SR04 GND** → **Arduino GND**
   - Black wire (always use black for ground)
   - Connect to any GND pin on Arduino

3. **HC-SR04 Trig** → **Arduino Pin 9**
   - Any colored wire (e.g., yellow)
   - This sends the trigger pulse

4. **HC-SR04 Echo** → **Arduino Pin 10**
   - Any colored wire (e.g., green)
   - This receives the echo signal

**Visual:**
```
HC-SR04          Arduino
VCC   ────────── 5V
GND   ────────── GND
Trig  ────────── Pin 9
Echo  ────────── Pin 10
```

---

## Step 3: Connect L293D Motor Driver

The L293D is a 16-pin IC. When looking at it with the notch on the left, pins are numbered:

```
     ┌──────────────┐
 1   │              │ 16
 2   │              │ 15
 3   │              │ 14
 4   │              │ 13
 5   │              │ 12
 6   │              │ 11
 7   │              │ 10
 8   │              │ 9
     └──────────────┘
```

### L293D Connections:

1. **L293D Pin 1 (Enable 1)** → **Arduino Pin 5**
   - Controls fan on/off and speed

2. **L293D Pin 2 (Input 1)** → **Arduino Pin 4**
   - Controls forward direction

3. **L293D Pin 7 (Input 2)** → **Arduino Pin 3**
   - Controls backward direction

4. **L293D Pin 3 (Output 1)** → **Fan Terminal 1**
   - One terminal of your DC fan

5. **L293D Pin 6 (Output 2)** → **Fan Terminal 2**
   - Other terminal of your DC fan

6. **L293D Pin 8 (VCC2)** → **External Power Supply +12V** (if using 12V fan)
   - OR connect to Arduino 5V if using 5V fan
   - This powers the motor

7. **L293D Pin 16 (VCC Logic)** → **Arduino 5V**
   - Powers the logic part of L293D

8. **L293D Pins 4, 5, 12, 13 (GND)** → **Arduino GND**
   - Connect all GND pins together
   - Also connect external power supply GND here

**Visual:**
```
L293D Pin        Arduino/Component
Pin 1  ────────── Arduino Pin 5
Pin 2  ────────── Arduino Pin 4
Pin 3  ────────── Fan Terminal 1
Pin 6  ────────── Fan Terminal 2
Pin 7  ────────── Arduino Pin 3
Pin 8  ────────── 12V Power Supply + (or 5V)
Pin 16 ────────── Arduino 5V
Pins 4,5,12,13 ─── Arduino GND (all together)
```

---

## Step 4: Connect DC Fan

Your DC fan has 2 terminals (wires). Connect them:

1. **Fan Terminal 1** → **L293D Pin 3 (Output 1)**
2. **Fan Terminal 2** → **L293D Pin 6 (Output 2)**

**Note:** If the fan spins in the wrong direction, just swap these two connections.

---

## Step 5: Power Connections

### Arduino Power:
- Connect **USB cable** from your laptop to Arduino
- This provides 5V power to Arduino

### Motor Power (if using 12V fan):
- Connect **12V power supply positive (+) wire** → **L293D Pin 8**
- Connect **12V power supply negative (-) wire** → **Arduino GND** (same as L293D GND)

**Important:** All grounds must be connected together:
- Arduino GND
- L293D GND (pins 4, 5, 12, 13)
- HC-SR04 GND
- External power supply GND (if used)

---

## Complete Connection Summary

### Arduino Pin Connections:
```
Arduino Pin 3  → L293D Pin 7  (Input 2)
Arduino Pin 4  → L293D Pin 2  (Input 1)
Arduino Pin 5  → L293D Pin 1  (Enable 1)
Arduino Pin 9  → HC-SR04 Trig
Arduino Pin 10 → HC-SR04 Echo
Arduino 5V     → L293D Pin 16 (VCC Logic)
Arduino 5V     → HC-SR04 VCC
Arduino GND    → L293D GND (pins 4,5,12,13)
Arduino GND    → HC-SR04 GND
```

### L293D Connections:
```
L293D Pin 1  → Arduino Pin 5
L293D Pin 2  → Arduino Pin 4
L293D Pin 3  → Fan Terminal 1
L293D Pin 6  → Fan Terminal 2
L293D Pin 7  → Arduino Pin 3
L293D Pin 8  → 12V Power Supply + (or 5V for 5V fan)
L293D Pin 16 → Arduino 5V
L293D GND    → Arduino GND
```

### Power Supply (for 12V fan):
```
12V Power Supply + → L293D Pin 8
12V Power Supply - → Arduino GND (common ground)
```

---

## Step-by-Step Assembly Instructions

### Using a Breadboard (Recommended):

1. **Place L293D on breadboard:**
   - Insert L293D IC into breadboard (straddle the center gap)
   - Make sure notch is on the left side

2. **Place HC-SR04 on breadboard:**
   - Insert sensor pins into breadboard

3. **Connect power rails:**
   - Connect Arduino 5V to breadboard positive rail (red line)
   - Connect Arduino GND to breadboard negative rail (blue line)

4. **Connect L293D:**
   - Pin 1 → Arduino Pin 5 (via breadboard)
   - Pin 2 → Arduino Pin 4 (via breadboard)
   - Pin 7 → Arduino Pin 3 (via breadboard)
   - Pin 16 → Breadboard positive rail (5V)
   - Pins 4, 5, 12, 13 → Breadboard negative rail (GND)

5. **Connect HC-SR04:**
   - VCC → Breadboard positive rail (5V)
   - GND → Breadboard negative rail (GND)
   - Trig → Arduino Pin 9 (via breadboard)
   - Echo → Arduino Pin 10 (via breadboard)

6. **Connect Fan:**
   - Terminal 1 → L293D Pin 3 (via breadboard)
   - Terminal 2 → L293D Pin 6 (via breadboard)

7. **Connect External Power (if needed):**
   - 12V + → L293D Pin 8 (via breadboard)
   - 12V - → Breadboard negative rail (GND)

### Without Breadboard (Direct Wiring):

1. **Solder or use jumper wires directly:**
   - Connect each component directly to Arduino pins
   - Use heat shrink tubing to protect connections
   - Make sure no wires are touching each other

---

## Visual Breadboard Layout

```
                    Arduino Uno
                    ┌─────────┐
                    │         │
    Pin 3 ──────────┤ 3       │
    Pin 4 ──────────┤ 4       │
    Pin 5 ──────────┤ 5       │
    Pin 9 ──────────┤ 9       │
    Pin 10 ─────────┤ 10      │
    5V ─────────────┤ 5V      │
    GND ────────────┤ GND     │
                    └─────────┘
                      │ │ │ │ │ │ │
                      │ │ │ │ │ │ │
        ┌─────────────┴─┴─┴─┴─┴─┴─┐
        │      Breadboard         │
        │                         │
        │  [L293D IC]             │
        │  [HC-SR04]              │
        │                         │
        │  Fan Wire 1 ────────────┤
        │  Fan Wire 2 ────────────┤
        │                         │
        │  12V + ─────────────────┤ (if needed)
        │  12V - ─────────────────┤ (if needed)
        └─────────────────────────┘
```

---

## Safety Checklist Before Powering On

- [ ] All connections are secure (no loose wires)
- [ ] No wires are touching each other (no short circuits)
- [ ] Power supply voltage matches fan requirements (5V or 12V)
- [ ] All GND connections are connected together
- [ ] L293D is oriented correctly (notch on left)
- [ ] USB cable is connected to Arduino
- [ ] External power supply is connected correctly (if used)
- [ ] Double-checked all pin numbers

---

## Testing Connections

### Test 1: Arduino Power
- Connect USB cable
- Arduino power LED should light up
- If not, check USB cable and port

### Test 2: Serial Communication
- Upload a simple program to Arduino
- Open Serial Monitor at 9600 baud
- You should see messages

### Test 3: Ultrasonic Sensor
- Upload code that reads sensor
- Check Serial Monitor for distance readings
- Wave hand in front of sensor, distance should change

### Test 4: Fan Control
- Upload code that controls fan
- Fan should turn on/off
- If fan doesn't spin, check:
  - Power supply voltage
  - L293D connections
  - Fan connections

---

## Common Mistakes to Avoid

1. **Wrong L293D orientation** - Notch must be on the left
2. **Reversed fan wires** - Fan spins wrong direction (just swap them)
3. **Missing ground connections** - All GND must be connected together
4. **Wrong voltage** - Check if fan is 5V or 12V
5. **Loose connections** - Make sure all wires are secure
6. **Short circuits** - Make sure no wires touch each other
7. **Wrong pin numbers** - Double-check Arduino pin numbers

---

## Troubleshooting

### Fan doesn't spin:
- Check if L293D Pin 8 has power (12V or 5V)
- Check if Arduino Pin 5 is sending signal
- Check fan connections to L293D Pin 3 and Pin 6
- Test fan directly with battery

### Sensor not working:
- Check if HC-SR04 has power (5V and GND)
- Check Trig and Echo connections
- Make sure nothing is blocking sensor
- Test with Serial Monitor

### Arduino not responding:
- Check USB connection
- Check if correct COM port is selected
- Try different USB cable
- Check if Arduino LED is on

---

## Quick Reference Card

Print this and keep it handy:

```
ARDUINO PINS:
Pin 3  → L293D Pin 7
Pin 4  → L293D Pin 2
Pin 5  → L293D Pin 1
Pin 9  → HC-SR04 Trig
Pin 10 → HC-SR04 Echo
5V     → L293D Pin 16, HC-SR04 VCC
GND    → L293D GND, HC-SR04 GND

L293D:
Pin 3  → Fan Terminal 1
Pin 6  → Fan Terminal 2
Pin 8  → 12V Power Supply +

ALL GROUNDS CONNECTED TOGETHER!
```

---

## Need Help?

If something doesn't work:
1. Check all connections against this guide
2. Verify pin numbers
3. Test each component individually
4. Check Serial Monitor for error messages
5. Make sure code is uploaded correctly

Good luck with your project! 🚀

