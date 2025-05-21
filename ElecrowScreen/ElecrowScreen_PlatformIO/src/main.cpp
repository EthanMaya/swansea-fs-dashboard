#include "can_rule_engine.h"
#include "driver/twai.h"
#include "ui.h"
#include "ui_code.hpp"
#include <ESP32-TWAI-CAN.hpp>
#include <lvgl.h>

// #define CAN_TX 44
// #define CAN_RX 43
#define CAN_TX 5
#define CAN_RX 4
#define SPEED 1000
#define HAS_DISPLAY 0
// #define P 43
// works   38 44 43
// doesn't 19
typedef u_int8_t u8;
typedef u_int16_t u16;

void handle_rpm(const CanFrame &rxFrame);
void handle_throttle(const CanFrame &rxFrame);
void handle_oil_pressure(const CanFrame &rxFrame);
void handle_oil_temp(const CanFrame &rxFrame);
void handle_gear_selection(const CanFrame &rxFrame);
void handle_accelerator_pedal(const CanFrame &rxFrame);

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
char buf[16];
CanFrame rxFrame;
RuleEngine<CanFrame> rule_engine;

void setup() {
  Serial.begin(9600);

  auto SUCCESS =
      ESP32Can.begin(ESP32Can.convertSpeed(SPEED), CAN_TX, CAN_RX, 10, 10);
  if (SUCCESS) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }

  rule_engine.add_rule(CompareIdentifier(0x360), &handle_rpm);
  rule_engine.add_rule(CompareIdentifier(0x360), &handle_throttle);
  rule_engine.add_rule(CompareIdentifier(0x361), &handle_oil_pressure);
  rule_engine.add_rule(CompareIdentifier(0x3E0), &handle_oil_temp);
  rule_engine.add_rule(CompareIdentifier(0x470), &handle_gear_selection);
  rule_engine.add_rule(CompareIdentifier(0x471), &handle_accelerator_pedal);

#if (HAS_DISPLAY)
  init_screen();
#endif
}

void loop() {
  if (ESP32Can.readFrame(rxFrame, 1000)) {
    Serial.println("In loop");
    rule_engine.run(rxFrame);
  }
}

void handle_rpm(const CanFrame &rxFrame) {
  // 0x360; bits 0-1 RPM; y = x
  u16 bit0 = rxFrame.data[0];
  u16 bit1 = rxFrame.data[1];
  u16 rpm_val = ((bit0 << 8) | bit1);
  lv_snprintf(buf, sizeof(buf), "%d", rpm_val);
#if (HAS_DISPLAY)
  // Update display
  ;
#else
  Serial.print("RPM: ");
  Serial.println(buf);
#endif
}

void handle_throttle(const CanFrame &rxFrame) {
  // 0x360; bits 4-5 Throttle Position; y = x/10
  u16 bit0 = rxFrame.data[4];
  u16 bit1 = rxFrame.data[5];
  u16 throttle_val = ((bit0 << 8) | bit1) / 10.0;
  lv_snprintf(buf, sizeof(buf), "%d", throttle_val);
#if (HAS_DISPLAY)
  // Update display
  ;
#else
  Serial.print("Throttle: ");
  Serial.println(buf);
#endif
}
void handle_oil_pressure(const CanFrame &rxFrame) {
  // 0x361; bits 2-3 Oil Pressure; y = x/10 - 101.3
  u16 bit0 = rxFrame.data[2];
  u16 bit1 = rxFrame.data[3];
  u16 oil_p_val = ((bit0 << 8) | bit1) / 10.0 - 101.3;
  lv_snprintf(buf, sizeof(buf), "%d", oil_p_val);
#if (HAS_DISPLAY)
  // Update display
  ;
#else
  Serial.print("Oil Pressure: ");
  Serial.println(buf);
#endif
}

void handle_oil_temp(const CanFrame &rxFrame) {
  // 0x3E0; bits 6-7; Oil temperature; y = x/10
  u16 bit0 = rxFrame.data[6];
  u16 bit1 = rxFrame.data[7];
  u16 oil_temp_val = ((bit0 << 8) | bit1) / 10.0;
  lv_snprintf(buf, sizeof(buf), "%d", oil_temp_val);
#if (HAS_DISPLAY)
  // Update display
  ;
#else
  Serial.print("Oil Temp: ");
  Serial.println(buf);
#endif
}

void handle_gear_selection(const CanFrame &rxFrame) {
  // 0x470; bits 7; gear position; enum val ??
  u16 gear_val = rxFrame.data[7];
  lv_snprintf(buf, sizeof(buf), "%d", gear_val);
#if (HAS_DISPLAY)
  // Update display
  ;
#else
  Serial.print("Gear: ");
  Serial.println(buf);
#endif
}

void handle_accelerator_pedal(const CanFrame &rxFrame) {
  // 0x471; bits 2-3 Accelerator pedal positon; y = x/10
  u16 bit0 = rxFrame.data[2];
  u16 bit1 = rxFrame.data[3];
  u16 accel_val = ((bit0 << 8) | bit1) / 10.0; // Merge bits
  lv_snprintf(buf, sizeof(buf), "%d", accel_val);
#if (HAS_DISPLAY)
  // Update display
  ;
#else
  Serial.print("Accel Pedal: ");
  Serial.println(buf);
#endif
}

