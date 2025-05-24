#include "can_rule_engine.h"
#include "can_rules.hpp"
#include "driver/twai.h"
#include "ui.h"
#include "ui_code.hpp"
#include <ESP32-TWAI-CAN.hpp>
#include <lvgl.h>

#define CAN_TX 44
#define CAN_RX 43
// #define CAN_TX 5
// #define CAN_RX 4
#define SPEED 1000
#define HAS_DISPLAY 1
// #define P 43
// works   38 44 43
// doesn't 19
typedef u_int8_t u8;
typedef u_int16_t u16;

class CompareIdentifier {
  u16 identifier;

public:
  CompareIdentifier(u16 identifier) : identifier(identifier){};
  bool operator()(const CanFrame &rxFrame) {
    return rxFrame.identifier == identifier;
  }
};

#if (HAS_DISPLAY)
// Define lvgl vars here
;
#endif

//
CanFrame rxFrame;
RuleEngine<CanFrame> rule_engine;
// ui_erpm            RPM   0x360 0-1 rpm y = x
// ui_evoltage        V     0x372 0-1 battery voltage Volts y = x/10
// ui_eoilpressure    P     0x361 0-1 oil pressure kPa y = x/10 - 101.3
// ui_eoiltemperature T     0x3E0 6-7 oil temp     K   y = x/10
// ui_egear           Gear  0x470 6 gear selector position enum
// ui_eengine         E     0x3E4 7:7 check engine light boolean 0=off, 1=on
// ui_espeed          Speed 0x370 0-1 vehicle speed km/h y = x/10

uint32_t my_tick_cb() { return millis(); }
void setup() {
  Serial.begin(9600);

  auto SUCCESS =
      ESP32Can.begin(ESP32Can.convertSpeed(SPEED), CAN_TX, CAN_RX, 10, 10);
  if (SUCCESS) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }

  rule_engine.add_rule(CompareIdentifier(0x370), &handle_speed);
  rule_engine.add_rule(CompareIdentifier(0x360), &handle_rpm);
  rule_engine.add_rule(CompareIdentifier(0x372), &handle_engine_voltage);
  rule_engine.add_rule(CompareIdentifier(0x361), &handle_oil_pressure);
  rule_engine.add_rule(CompareIdentifier(0x3E0), &handle_oil_temp);
  rule_engine.add_rule(CompareIdentifier(0x470), &handle_gear_selection);
  rule_engine.add_rule(CompareIdentifier(0x3E4), &handle_engine_light);

#if (HAS_DISPLAY)
  init_screen();
#endif
}
const uint16_t GUI_PERIOD_MS = 5; // refresh LVGL every 10 ms
static uint32_t last_gui = 0;
static uint32_t last_tick = 0;

void loop() {
  if (ESP32Can.readFrame(rxFrame, 0)) {
    Serial.println("In loop");
    rule_engine.run(rxFrame);
  }
  lv_timer_handler_run_in_period(GUI_PERIOD_MS);
}
