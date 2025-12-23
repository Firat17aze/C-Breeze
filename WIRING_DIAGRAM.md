# Wiring Diagram - IoT Smart Fan Control System

## Complete Circuit Diagram

```
                    ┌─────────────────┐
                    │   Arduino Uno   │
                    │   (ATmega328P)  │
                    └────────┬────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
        │                    │                    │
    ┌───▼───┐          ┌─────▼─────┐        ┌───▼────┐
    │ HC-SR04│          │   L293D   │        │  USB   │
    │Sensor │          │  Motor    │        │  Cable │
    └───┬───┘          │  Driver   │        └───┬────┘
        │              └─────┬─────┘              │
        │                    │                   │
    ┌───┴───┐          ┌─────┴─────┐        ┌───┴────┐
    │       │          │           │        │        │
    │  Fan  │          │   DC Fan  │        │ Laptop │
    │       │          │           │        │        │
    └───────┘          └───────────┘        └────────┘
```

## Detailed Pin Connections

### Arduino Uno Pin Layout

```
                    ┌─────┬─────┐
                 ┌──│ 0   │ 13  │──┐
                 │  │ 1   │ 12  │  │
                 │  │ 2   │ 11  │  │
                 │  │ 3◄──│ 10◄─┤  │ (PD3, PB2)
                 │  │ 4◄──│  9◄─┤  │ (PD4, PB1)
                 │  │ 5◄──│  8  │  │ (PD5)
                 │  │ 6   │  7  │  │
                 └──┴─────┴─────┴──┘
                      │     │
                      │     │
                   GND    VCC
```

### HC-SR04 Ultrasonic Sensor

```
HC-SR04 Pinout:
┌─────────────┐
│   VCC  ─────┼───► 5V (Arduino)
│   Trig ─────┼───► Pin 9 (PB1)
│   Echo ─────┼───► Pin 10 (PB2)
│   GND  ─────┼───► GND (Arduino)
└─────────────┘
```

### L293D Motor Driver

```
L293D Pinout (Top View):
     ┌──────────────┐
 1   │ Enable 1     │ 16  VCC (5V Logic)
 2   │ Input 1 ◄────┼───── Pin 4 (PD4)
 3   │ Output 1 ────┼───── Motor Terminal 1
 4   │ GND          │
 5   │ GND          │
 6   │ Output 2 ────┼───── Motor Terminal 2
 7   │ Input 2 ◄────┼───── Pin 3 (PD3)
 8   │ VCC2 ────────┼───── Motor Power (12V)
     │              │
 9   │ Enable 2     │ (Not used)
10   │ Input 3      │ (Not used)
11   │ Output 3     │ (Not used)
12   │ GND          │
13   │ GND          │
14   │ Output 4     │ (Not used)
15   │ Input 4      │ (Not used)
16   │ VCC (Logic)  │ 5V
     └──────────────┘

Note: Enable 1 is connected to Pin 5 (PD5) for PWM speed control
```

### DC Fan Connection

```
DC Fan:
┌─────────────┐
│ Terminal 1 ──┼───► L293D Output 1 (Pin 3)
│ Terminal 2 ──┼───► L293D Output 2 (Pin 6)
└─────────────┘
```

## Power Supply Connections

### Arduino Power
- **USB Cable**: Provides 5V to Arduino (from laptop)
- **VIN Pin**: Can use external 7-12V supply (optional)

### Motor Power
- **L293D VCC2 (Pin 8)**: Connect to external power supply
  - For 12V fan: Use 12V adapter
  - For 5V fan: Can use 5V from Arduino (not recommended for high current)

### Logic Power
- **L293D VCC (Pin 16)**: Connect to Arduino 5V
- **HC-SR04 VCC**: Connect to Arduino 5V

### Ground Connections
- All GND pins must be connected together:
  - Arduino GND
  - L293D GND (Pins 4, 5, 12, 13)
  - HC-SR04 GND
  - External power supply GND (if used)

## Breadboard Layout

```
                    Power Rails
    ┌─────────────────────────────────────┐
    │ +5V  │  GND  │  +12V │  GND         │
    └──┬───┴───┬───┴───┬───┴───┬──────────┘
       │       │       │       │
       │       │       │       │
    ┌──▼───────────────────────▼──┐
    │                              │
    │  [Arduino]                   │
    │                              │
    │  Pin 3 ────► [L293D Pin 7]  │
    │  Pin 4 ────► [L293D Pin 2]  │
    │  Pin 5 ────► [L293D Pin 1]  │
    │  Pin 9 ────► [HC-SR04 Trig] │
    │  Pin 10 ───► [HC-SR04 Echo] │
    │                              │
    │  [L293D Pin 3] ───► [Fan +] │
    │  [L293D Pin 6] ───► [Fan -] │
    │                              │
    └──────────────────────────────┘
```

## Connection Checklist

- [ ] Arduino Pin 3 (PD3) → L293D Pin 7 (Input 2)
- [ ] Arduino Pin 4 (PD4) → L293D Pin 2 (Input 1)
- [ ] Arduino Pin 5 (PD5) → L293D Pin 1 (Enable 1)
- [ ] Arduino Pin 9 (PB1) → HC-SR04 Trigger
- [ ] Arduino Pin 10 (PB2) → HC-SR04 Echo
- [ ] L293D Pin 3 (Output 1) → Fan Terminal 1
- [ ] L293D Pin 6 (Output 2) → Fan Terminal 2
- [ ] L293D Pin 8 (VCC2) → External 12V (if using 12V fan)
- [ ] L293D Pin 16 (VCC) → Arduino 5V
- [ ] HC-SR04 VCC → Arduino 5V
- [ ] All GND pins connected together
- [ ] USB cable connected to Arduino and laptop

## Safety Notes

1. **Power Supply**: Ensure external power supply matches fan voltage requirements
2. **Current Rating**: Check that power supply can handle fan current (typically 0.5-2A)
3. **Polarity**: Double-check all connections before powering on
4. **Heat Sink**: L293D may need heat sink for continuous operation
5. **Isolation**: Keep motor power supply separate from logic power supply
6. **Fuses**: Consider adding fuses for protection

## Testing Connections

1. **Test Arduino**: Upload a simple blink program
2. **Test Serial**: Open Serial Monitor at 9600 baud
3. **Test Sensor**: Measure distance with sensor
4. **Test Motor**: Manually control motor with simple code
5. **Test Integration**: Run full system

## Common Wiring Mistakes

- ❌ Reversed motor terminals (fan spins wrong direction)
- ❌ Wrong enable pin connection
- ❌ Missing ground connections
- ❌ Incorrect voltage levels
- ❌ Loose connections
- ❌ Short circuits

## Visual Reference

For a visual wiring diagram, you can use:
- Fritzing (https://fritzing.org/)
- Tinkercad Circuits (https://www.tinkercad.com/circuits)
- Draw.io (https://app.diagrams.net/)

Create a diagram with:
- Arduino Uno board
- L293D IC (16-pin DIP)
- HC-SR04 sensor
- DC fan symbol
- All connections labeled

