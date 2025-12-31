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
