#include <hidboot.h>
#include "ImprovedMouse.h"

#define CHECK_BIT(var, pos) ((var)&pos)

// Updated to reflect your packet structure: btns wheel x_lo x_hi y_lo y_hi unused unused
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

class MouseRptParser : public MouseReportParser
{
  union
  {
    MYMOUSEINFO mouseInfo;
    uint8_t bInfo[sizeof(MYMOUSEINFO)];
  } prevState;

protected:
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);

  void OnMouseMove(MYMOUSEINFO *mi);
  void OnWheelMove(MYMOUSEINFO *mi);
};
