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
char buf[16];
CanFrame rxFrame;
RuleEngine<CanFrame> rule_engine;
// ui_erpm            RPM   0x360 0-1 rpm y = x
// ui_evoltage        V     0x372 0-1 battery voltage Volts y = x/10
// ui_eoilpressure    P     0x361 0-1 oil pressure kPa y = x/10 - 101.3
// ui_eoiltemperature T     0x3E0 6-7 oil temp     K   y = x/10
// ui_egear           Gear  0x470 6 gear selector position enum
// ui_eengine         E     0x3E4 7:7 check engine light boolean 0=off, 1=on
// ui_espeed          Speed 0x370 0-1 vehicle speed km/h y = x/10

boolean update = false;

boolean engine_error = false;
boolean engine_switch = false;

boolean speed = false;
u16 speed_value = 0;

boolean rpm_up = false;
boolean rpm_up_switch = false;
boolean rpm_down = false;
boolean rpm_down_switch = false;
u16 rpm_min = 225;
u16 rpm_max = 675;
u16 rpm_value = 0;

boolean temperature = false;
boolean temperature_switch = false;
u16 temperature_max = 100;
u16 temperature_value = 0;

boolean pressure = false;
boolean pressure_switch = false;
u16 pressure_min = 60;
u16 pressure_value = 0;

boolean voltage = false;
boolean voltage_switch = false;
u16 voltage_min = 60;
u16 voltage_value = 0;

boolean gear = false;
u16 gear_value = 0;

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
void update_text(u16 value, u16 &previous_value, lv_obj_t *ui_element) {
  if (value != previous_value) {
    previous_value = value;
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(ui_element, buf);
    update = true;
  }
}
void toggle_min_threshold(u16 value, u16 min_value, boolean &condition) {
  if (value < min_value) {
    condition = true;
  } else {
    condition = false;
  }
}
void toggle_max_threshold(u16 value, u16 max_value, boolean &condition) {
  if (value > max_value) {
    condition = true;
  } else {
    condition = false;
  }
}
void dim_text(boolean condition, lv_obj_t *ui_element) {
  if (condition) {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0xFFFFFF),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  } else {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0x555555),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  }
}
void toggle_visibility(boolean condition, boolean &toggle_flag,
                       lv_obj_t *ui_element) {
  if (condition) {
    toggle_flag = !toggle_flag;
    update = true;
  } else if (toggle_flag == true) {
    toggle_flag = false;
    update = true;
  }

  if (toggle_flag) {
    lv_obj_clear_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
  }
}

void display_update() {
  toggle_visibility(rpm_up, rpm_up_switch, ui_erpmbackswitchup);
  toggle_visibility(rpm_down, rpm_down_switch, ui_erpmbackswitchdown);
  toggle_visibility(engine_error, engine_switch, ui_eengineback);
  toggle_visibility(temperature, temperature_switch, ui_eoiltemperatureback);
  toggle_visibility(pressure, pressure_switch, ui_eoilpressureback);
  toggle_visibility(voltage, voltage_switch, ui_evoltageback);
  Serial.println(buf);
}
void update_speed_ui(u16 speed_val) {
  //  ui_espeed
  //  ui_espeedarc
  update_text(speed_val, speed_value, ui_espeed);
  lv_arc_set_value(ui_espeedarc, speed_val);
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

void update_rpm_ui(u16 rpm_val) {
  //  ui_erpm
  //  ui_erpmbackswitchdown
  //  ui_erpmbackswitchup
  //  ui_erpmbar
  update_text(rpm_val, rpm_value, ui_erpm);
  lv_bar_set_value(ui_erpmbar, rpm_val, LV_ANIM_OFF);
  toggle_max_threshold(rpm_val, rpm_max, rpm_up);
  toggle_min_threshold(rpm_val, rpm_min, rpm_down);
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

void update_engine_voltage_ui(u16 voltage_val) {
  //  ui_evoltage
  //  ui_evoltageback
  update_text(voltage_val, voltage_value, ui_evoltage);
  toggle_min_threshold(voltage_val, voltage_min, voltage);
  dim_text(voltage, ui_evoltage);
  dim_text(voltage, ui_voltagedu);
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

void update_oil_pressure_ui(u16 pressure_val) {
  //  ui_eoilpressure
  //  ui_eoilpressureback
  update_text(pressure_val, pressure_value, ui_eoilpressure);
  toggle_min_threshold(pressure_val, pressure_min, pressure);
  dim_text(pressure, ui_eoilpressure);
  dim_text(pressure, ui_oilpressuredu);
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

void update_oil_temperature_ui(u16 temperature_val) {
  //  ui_eoiltemperature
  //  ui_eoiltemperatureback
  update_text(temperature_val, temperature_value, ui_eoiltemperature);
  toggle_max_threshold(temperature_val, temperature_max, temperature);
  dim_text(temperature, ui_eoiltemperature);
  dim_text(temperature, ui_oiltemperaturedu);
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
void update_gear_ui(u16 gear_val) {
  // ui_egear
  update_text(gear_val, gear_value, ui_egear);
}

void handle_gear_selection(const CanFrame &rxFrame) {
  //  0x470; bits 7; gear position, enum; enum val ??
  u16 gear_val = rxFrame.data[6];
#if (HAS_DISPLAY)
  update_gear_ui(gear_val);
#else
#endif
}

void update_engine_light_ui(u16 engine_light_val) {
  //  ui_eengine
  //  ui_eengineback
  if (engine_light_val == 1) {
    engine_error = true;
  } else {
    engine_error = false;
  }
  toggle_max_threshold(engine_light_val, 0, engine_error);
  dim_text(engine_error, ui_enginedu);
}
void handle_engine_light(const CanFrame &rxFrame) {
  //  0x3E4 bits 7:7 check engine light boolean 0=off, 1=on
  u8 engine_light_val = (rxFrame.data[7] >> 7) & 0x01;
#if (HAS_DISPLAY)
  update_engine_light_ui(engine_light_val);
#else
#endif
}
