# Smart-Gate-Arduino

An Arduino-controlled smart gate that opens automatically when a vehicle approaches, or on command from a phone over Bluetooth. A light sensor adds a night-time safety mode: after dark, the gate opens only by Bluetooth command, not when something comes near it.

## Features

- **Automatic opening**: an ultrasonic sensor detects a vehicle within 10 cm and opens the gate.
- **Bluetooth remote control**: send `1` from a phone (for example a Bluetooth serial terminal app) to open the gate. Send `0`, or any other character, to close it again.
- **Day/night mode**: a light sensor (LDR) checks the ambient light. When it is dark, the distance sensor is ignored and the gate opens only by Bluetooth.
- **Status feedback**:
  - 16x2 I2C LCD shows `Gate open!` or `Gate close!`
  - Green LED when open, red LED when closed
  - The buzzer beeps for 1 second whenever the gate opens or closes
- **Serial debugging**: distance and light readings are printed to the Serial Monitor at 9600 baud.

## How it works

Once a second, the sketch reads the distance sensor, the light sensor and any incoming Bluetooth command, then decides whether the gate should be open:

| Light   | Vehicle within 10 cm | Bluetooth `1` | Gate     |
|---------|:--------------------:|:-------------:|----------|
| Day     | yes                  | any           | Opens    |
| Day     | any                  | yes           | Opens    |
| Night   | any                  | yes           | Opens    |
| Night   | yes                  | no            | Stays closed |
| Any     | no                   | no            | Closes   |

The servo turns to 90° to open and back to 0° to close. The LEDs, LCD and buzzer update only when the gate changes state.

The last Bluetooth command is remembered, so after `1` the gate stays open until a different character is sent.

## Hardware

| Component                          | Arduino pin                  |
|------------------------------------|------------------------------|
| Ultrasonic sensor (HC-SR04) TRIG   | D4                           |
| Ultrasonic sensor (HC-SR04) ECHO   | D2                           |
| Bluetooth module (HC-05/HC-06) TX  | D6 (Arduino RX)              |
| Bluetooth module (HC-05/HC-06) RX  | D5 (Arduino TX)              |
| Servo motor signal                 | D10                          |
| Red LED                            | D11                          |
| Green LED                          | D12                          |
| Buzzer                             | D13                          |
| Light sensor (LDR voltage divider) | A0                           |
| 16x2 LCD with I2C backpack         | SDA/SCL (A4/A5 on an Uno), address `0x27` |

Built for an Arduino Uno or a compatible board. Use current-limiting resistors on the LEDs, and power the servo from a supply that can handle its current draw.

## Software

### Libraries

- `Servo`, `Wire` and `SoftwareSerial`: included with the Arduino IDE
- `LiquidCrystal_I2C`: install it from the Arduino Library Manager

### Upload

1. Open `SmartGateArduino.ino` in the Arduino IDE.
2. Install the `LiquidCrystal_I2C` library.
3. Select your board and port, then upload.
4. Pair your phone with the Bluetooth module and connect with a serial terminal app at 9600 baud.

## Configuration

These constants at the top of the sketch can be adjusted:

| Constant            | Default | Meaning                                                    |
|---------------------|---------|------------------------------------------------------------|
| `detectionDistance` | `10`    | Distance in cm that counts as a vehicle at the gate        |
| `lightThreshold`    | `400`   | Analog reading (0–1023) below which it is considered night |

If your LCD does not display anything, check its I2C address (commonly `0x27` or `0x3F`) and update it in `LiquidCrystal_I2C lcd(0x27, 16, 2);`.

Calibrate `lightThreshold` by watching the `Light Level` values in the Serial Monitor in day and night conditions, since they depend on your LDR and resistor.
