#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include <usbhub.h>

#include "hidcustom.h"

// USB task timing - optimized for lower latency
unsigned long lastUsbTask = 0;
const unsigned long USB_TASK_INTERVAL = 0; // Reduced to 0 to process USB tasks as fast as possible

// Mouse movement accumulation to smooth output
int8_t accumulatedX = 0;
int8_t accumulatedY = 0;
int8_t accumulatedWheel = 0;
bool moveNeeded = false;

// Button mapping - based on the HID report descriptor
// Standard 5-button mouse uses bits 0-4 in the first byte
// Standard mapping: bit 0 = left, bit 1 = right, bit 2 = middle
// Side buttons are bits 3 and 4
#define BUTTON_LEFT    0x01  // Bit 0 (1)
#define BUTTON_RIGHT   0x02  // Bit 1 (2)
#define BUTTON_MIDDLE  0x04  // Bit 2 (4)
#define BUTTON_BACK    0x08  // Bit 3 (8)
#define BUTTON_FORWARD 0x10  // Bit 4 (16)

USB Usb;
USBHub Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb);
MouseRptParser Prs;

void setup()
{   
//    Serial.begin(57600);
//    Serial.setTimeout(1);
    Usb.Init();
    HidMouse.SetReportParser(0, &Prs);
    Mouse.begin();
}

void loop()
{
    unsigned long currentTime = millis();
    
    // Process USB tasks immediately without delay
    Usb.Task();
    lastUsbTask = currentTime;
    
    // Apply accumulated movements immediately when needed
    if (moveNeeded) {
        Mouse.move(accumulatedX, accumulatedY, accumulatedWheel);
        accumulatedX = 0;
        accumulatedY = 0;
        accumulatedWheel = 0;
        moveNeeded = false;
    }
    
    // Minimal delay to prevent CPU hogging - reduced for lower latency
    delayMicroseconds(50);
}

void MouseRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
    // Only process if buffer has enough data
    if (len < 6) return;
    
    // Based on your packet format: btns wheel x_lo x_hi y_lo y_hi unused unused
    MYMOUSEINFO *pmi = reinterpret_cast<MYMOUSEINFO*>(buf);

    // Handle all 5 standard mouse buttons
    // Map the physical buttons directly to their corresponding Arduino Mouse constants
    ProcessButton(prevState.mouseInfo.buttons, pmi->buttons, BUTTON_LEFT, MOUSE_LEFT);
    ProcessButton(prevState.mouseInfo.buttons, pmi->buttons, BUTTON_RIGHT, MOUSE_RIGHT);
    ProcessButton(prevState.mouseInfo.buttons, pmi->buttons, BUTTON_MIDDLE, MOUSE_MIDDLE);
    ProcessButton(prevState.mouseInfo.buttons, pmi->buttons, BUTTON_BACK, MOUSE_PREV);
    ProcessButton(prevState.mouseInfo.buttons, pmi->buttons, BUTTON_FORWARD, MOUSE_NEXT);

    // Handle X and Y movement - accumulate values
    if (pmi->dX || pmi->dY)
    {
        OnMouseMove(pmi);
    }

    if (pmi->wheel)
    {
        OnWheelMove(pmi);
    }

    // Store the previous state (only the bytes we care about)
    for (uint8_t i = 0; i < min(len, sizeof(MYMOUSEINFO)); i++) {
        prevState.bInfo[i] = buf[i];
    }
}

void MouseRptParser::OnMouseMove(MYMOUSEINFO *mi)
{
    // Process movement - use signed int8_t for deltaX/Y to handle negative movement
    int8_t deltaX = (int8_t)mi->dX;
    int8_t deltaY = (int8_t)mi->dY;
    
    // Accumulate movement instead of applying immediately
    accumulatedX += deltaX;
    accumulatedY += deltaY;
    moveNeeded = true;
}

void MouseRptParser::OnWheelMove(MYMOUSEINFO *mi)
{
    // Accumulate wheel movement
    accumulatedWheel += mi->wheel;
    moveNeeded = true;
}

// Process button state changes and apply the appropriate mouse action
inline void ProcessButton(uint8_t prevState, uint8_t newState, uint8_t buttonBit, uint8_t reportButton)
{
    bool prevPressed = (prevState & buttonBit) != 0;
    bool newPressed = (newState & buttonBit) != 0;

    if (prevPressed != newPressed)
    {
        if (newPressed)
        {
            Mouse.press(reportButton);
        } 
        else
        {
            Mouse.release(reportButton);
        }
    }
}
