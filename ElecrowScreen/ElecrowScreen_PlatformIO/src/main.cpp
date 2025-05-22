#include "can_rule_engine.h"
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

void handle_speed(const CanFrame &rxFrame);
void handle_rpm(const CanFrame &rxFrame);
void handle_engine_voltage(const CanFrame &rxFrame);
void handle_oil_pressure(const CanFrame &rxFrame);
void handle_oil_temp(const CanFrame &rxFrame);
void handle_gear_selection(const CanFrame &rxFrame);
void handle_engine_light(const CanFrame &rxFrame);
void display_update();

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

void loop() {
  update = false;
  if (ESP32Can.readFrame(rxFrame, 1000)) {
    Serial.println("In loop");
    rule_engine.run(rxFrame);
  }
  display_update();
  if (update) {
    lv_timer_handler();
    delay(10);
  }
}

void handle_speed(const CanFrame &rxFrame) {
  //  0x370; bits 0-1 vehicle speed km/h y = x/10
  u16 bit0 = rxFrame.data[0];
  u16 bit1 = rxFrame.data[1];
  u16 speed_val = ((bit0 << 8) | bit1) / 10;
#if (HAS_DISPLAY)
  update_speed_ui(speed_val);
#else
#endif
}

void handle_rpm(const CanFrame &rxFrame) {
  // 0x360; bits 0-1 RPM; y = x
  u16 bit0 = rxFrame.data[0];
  u16 bit1 = rxFrame.data[1];
  u16 rpm_val = ((bit0 << 8) | bit1);
#if (HAS_DISPLAY)
  update_rpm_ui(rpm_val);
#else
#endif
}

void handle_oil_pressure(const CanFrame &rxFrame) {
  //  0x361; bits 2-3 Oil Pressure KPa; y = x/10 - 101.3
  u16 bit0 = rxFrame.data[2];
  u16 bit1 = rxFrame.data[3];
  u16 pressure_val = ((bit0 << 8) | bit1) / 10.0 - 101.3;
#if (HAS_DISPLAY)
  update_oil_pressure_ui(pressure_val);
#else
#endif
}

void handle_engine_voltage(const CanFrame &rxFrame) {
  //  0x372; bits 0-1 battery voltage Volts; y = x/10
  u16 bit0 = rxFrame.data[0];
  u16 bit1 = rxFrame.data[1];
  u16 voltage_val = ((bit0 << 8) | bit1) / 10.0;
#if (HAS_DISPLAY)
  update_engine_voltage_ui(voltage_val);
#else
#endif
}

void handle_oil_temp(const CanFrame &rxFrame) {
  //  0x3E0; bits 6-7; Oil temperature K; y = x/10
  u16 bit0 = rxFrame.data[6];
  u16 bit1 = rxFrame.data[7];
  u16 temperature_val = ((bit0 << 8) | bit1) / 10.0;
#if (HAS_DISPLAY)
  update_oil_temperature_ui(temperature_val);
#else
#endif
}
void handle_gear_selection(const CanFrame &rxFrame) {
  //  0x470; bits 7; gear position, enum; enum val ??
  u16 gear_val = rxFrame.data[6];
#if (HAS_DISPLAY)
  update_gear_ui(gear_val);
#else
#endif
}

void handle_engine_light(const CanFrame &rxFrame) {
  //  0x3E4 bits 7:7 check engine light boolean 0=off, 1=on
  u8 engine_light_val = (rxFrame.data[7] >> 7) & 0x01;
#if (HAS_DISPLAY)
  update_engine_light_ui(engine_light_val);
#else
#endif
}
