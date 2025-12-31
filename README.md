# Arduino 4x4 Keypad Auto Row/Column Mapper

Utility sketch for Arduino that automatically detects **rowPins** and **colPins**
of a standard **4×4 matrix keypad** when wiring is unknown.

The sketch asks the user to press all 16 keys in a predefined order and then
prints correct `rowPins[]` and `colPins[]` arrays ready to be used with the
Keypad library.

---

## Why this exists

Most 4×4 keypads are sold without documentation.
Pin order varies between manufacturers, and guessing row/column wiring is
time-consuming.

This sketch solves the problem **once and for all**.

---

## Features

- Works without the Keypad library
- Detects real electrical connections (not assumptions)
- Automatically calculates:
  - `rowPins[4]` (top → bottom)
  - `colPins[4]` (left → right)
- Outputs result to Serial Monitor
- Optional LCD1602 (I2C) support
- Designed for Arduino Nano / Uno

---

## Supported hardware

- Arduino Nano / Uno
- 4×4 matrix keypad (8-wire, passive)
- Optional: LCD1602 with I2C backpack

---

## Pin assumptions (default)

Keypad lines connected to:
D2, D3, D4, D5, D6, D7, D8, D9


You can change this easily in the source code.

---

## How to use

1. Connect keypad lines to Arduino pins (order does not matter).
2. Upload the sketch.
3. Open Serial Monitor at **9600 baud**.
4. Press keys in this exact order when prompted:

1 2 3 A 4 5 6 B 7 8 9 C * 0 # D

5. After the last key, the sketch prints ready-to-use arrays

```
byte rowPins[4] = {...};
byte colPins[4] = {...};
```
```
Example output
=== RESULT ===
byte rowPins[ROWS] = {5, 4, 3, 2};
byte colPins[COLS] = {9, 8, 7, 6};
```
6. Copy these arrays into your project using the Keypad library.
