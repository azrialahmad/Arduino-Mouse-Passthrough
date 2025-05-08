# Arduino Mouse Passthrough

This project creates a USB host-to-device mouse passthrough using an Arduino board (Leonardo/Micro/Pro Micro). It allows connecting a USB mouse to the Arduino, which then appears as a separate HID device to the computer, passing through all movements and button presses.

## Features

- Support for all 5 standard mouse buttons (left, right, middle, and side buttons)
- 16-bit mouse movement precision (high-resolution tracking)
- Optimized USB task handling
- Smooth tracking for high precision movements

## Hardware Requirements

- Arduino Leonardo, Micro, or Pro Micro (ATmega32U4-based board)
- USB Host Shield
- USB Mouse (tested with WLMouse Beast x Mini)

## Installation

1. Install the required libraries:
   - USB Host Shield 2.0 Library
   - HID Library (included with Arduino IDE)

2. Connect the USB Host Shield to your Arduino board
3. Upload the sketch to your Arduino
4. Connect your USB mouse to the USB Host Shield
5. Your Arduino will now act as a mouse, passing through all commands from the original mouse

## Important: Mouse HID Report Structure

The most critical part of adapting this code to your specific mouse is ensuring the `MYMOUSEINFO` struct in `hidcustom.h` matches your mouse's HID report format.

### Current Structure for WLMouse Beast x Mini

The current implementation is configured for the WLMouse Beast x Mini which uses this HID report structure:

```cpp
struct MYMOUSEINFO
{
  uint8_t buttons;  // First byte - buttons
  int8_t wheel;     // Second byte - wheel
  uint8_t dX;       // Third byte - X low byte
  uint8_t dXHi;     // Fourth byte - X high byte
  uint8_t dY;       // Fifth byte - Y low byte
  uint8_t dYHi;     // Sixth byte - Y high byte
  uint8_t unused1;  // Seventh byte - unused
  uint8_t unused2;  // Eighth byte - unused
};
```

This structure supports 16-bit mouse movement (using both low and high bytes for X and Y), allowing for much higher precision tracking than standard 8-bit mouse movement.

### Adapting to Your Mouse

Different mice may send HID reports in different formats. To adapt this code to your mouse:

1. Use a USB analyzer or HID report viewer to examine your mouse's data format
2. Modify the `MYMOUSEINFO` struct to match your mouse's byte arrangement
3. Update the parsing code in `Parse()`, `OnMouseMove()`, and `OnWheelMove()` functions if necessary

Common variations include:
- Different ordering of bytes (e.g., X/Y before wheel)
- Different data types (signed vs unsigned)
- Additional bytes for high-resolution mice
- Different button bit mappings

## Button Mapping

The current implementation maps buttons as follows:
- BUTTON_LEFT (0x01): Left click
- BUTTON_RIGHT (0x02): Right click
- BUTTON_MIDDLE (0x04): Middle click/scroll wheel click
- BUTTON_BACK (0x08): Back button (typically side button)
- BUTTON_FORWARD (0x10): Forward button (typically side button)

## Checking Your Mouse's HID Report

To determine your mouse's HID report structure:
1. On Windows, use the USB Device Tree Viewer or USBDeview
2. On Linux, use `lsusb -v` or `usbhid-dump`
3. On macOS, use the "USB Prober" tool or IORegistryExplorer

Look for the HID Report Descriptor and analyze the input report format.

## Latency Optimization

This code is optimized for minimal latency:
- Removes Serial communications
- Processes USB tasks immediately
- Uses minimal delays
- Efficiently processes mouse movements

## Credits

This project is based on [SunOner's HID_Arduino repository](https://github.com/SunOner/HID_Arduino/), modified to support additional buttons, 16-bit high-precision mouse movement

## License

This project is licensed under the MIT License - see the LICENSE file for details.
