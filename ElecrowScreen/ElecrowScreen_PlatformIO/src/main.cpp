/*
########################## open include directory and edit config.h to change values ############################
*/
#include "can_rule_engine.h"
#include "driver/twai.h"
#include "ui.h"
#include "ui_code.hpp"
#include <ESP32-TWAI-CAN.hpp>
#include <lvgl.h>
#include "config.h"

#define CAN_TX 44
#define CAN_RX 43
//#define CAN_TX 5
//#define CAN_RX 4
#define SPEED 1000
#define HAS_DISPLAY 1
// #define P 43
// works   38 44 43
// doesn't 19
typedef u_int8_t u8;
typedef u_int16_t u16;

void handle_rpm(const CanFrame &rxFrame);
void handle_engine_voltage(const CanFrame &rxFrame);
void handle_oil_pressure(const CanFrame &rxFrame);
void handle_oil_temp(const CanFrame &rxFrame);
void handle_gear_selection(const CanFrame &rxFrame);
void display_update();

class CompareIdentifier {
  u16 identifier;

public:
  CompareIdentifier(u16 identifier) : identifier(identifier){};
  bool operator()(const CanFrame &rxFrame) {
    return rxFrame.identifier == identifier;
  }
};


//
char buf[16];
CanFrame rxFrame;
RuleEngine<CanFrame> rule_engine;


boolean update = false;

boolean rpm_up = false;
boolean rpm_down = false;
boolean temperature = false;
boolean pressure = false;
boolean voltage = false;
boolean gear = false;

void setup()
{
  Serial.begin(9600);

  auto SUCCESS =
      ESP32Can.begin(ESP32Can.convertSpeed(SPEED), CAN_TX, CAN_RX, 10, 10);
  if (SUCCESS) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }

  rule_engine.add_rule(CompareIdentifier(0x470), &handle_gear_selection);
  rule_engine.add_rule(CompareIdentifier(0x360), &handle_rpm);
  rule_engine.add_rule(CompareIdentifier(0x372), &handle_engine_voltage);
  rule_engine.add_rule(CompareIdentifier(0x361), &handle_oil_pressure);
  rule_engine.add_rule(CompareIdentifier(0x3E0), &handle_oil_temp);

#if (HAS_DISPLAY)
  init_screen();
#endif
}

void loop() {
  update = false;
  if (ESP32Can.readFrame(rxFrame, 1000))
  {
    //Serial.println("In loop");
    rule_engine.run(rxFrame);
  }
  display_update();
  if (update)
  {
    lv_timer_handler();
    //delay(10);
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
  if (value > max_value) 
  {
    condition = true;
  }
  else 
  {
    condition = false;
  }
  
}
void dim_text(boolean condition, lv_obj_t *ui_element) {
  if (condition) 
  {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
  }
  else
  {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
  }

}
void update_text(u16 value, lv_obj_t *ui_element) {
  lv_snprintf(buf, sizeof(buf), "%d", value);
  lv_label_set_text(ui_element, buf);
  update = true;
}
bool is_visible(lv_obj_t *o) { return !lv_obj_has_flag(o, LV_OBJ_FLAG_HIDDEN); }
void toggle_visibility(boolean condition, lv_obj_t *ui_element) {
  boolean toggle_flag = is_visible(ui_element); //true clear
  if (condition)
  {
    update = true;
  }
  else if (toggle_flag == false)
  {
    toggle_flag = true;
    update = true;
  }

  if (update) 
  {
    if (toggle_flag) 
    {
      lv_obj_add_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
    } 
    else 
    {
      lv_obj_clear_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
    }
    }
}

/*
Voltage works
Temperature Works Change to Celcius
Pressure Works kPa
RPM Works
Gear Works but is slow?

Check Engine Light Doesn't Work: issue, it's always on

Speed Don't Know
*/

void display_update() {

  toggle_visibility(rpm_up, ui_erpmbackswitchup);
  toggle_visibility(rpm_down, ui_erpmbackswitchdown);
  toggle_visibility(temperature, ui_eoiltemperatureback);
  toggle_visibility(pressure, ui_eoilpressureback);
  toggle_visibility(voltage, ui_evoltageback);
}




void handle_rpm(const CanFrame &rxFrame) {
    //ui_erpm            RPM   0x360 0-1 rpm y = x
    // ui_erpm
    // ui_erpmbackswitchdown
    // ui_erpmbackswitchup
    // ui_erpmbar
  // 0x360; bits 0-1 RPM; y = x
  u16 bit0 = rxFrame.data[0];
  u16 bit1 = rxFrame.data[1];
  u16 rpm_val = ((bit0 << 8) | bit1);
  update_text(rpm_val, ui_erpm);
  lv_bar_set_value(ui_erpmbar, rpm_val, LV_ANIM_OFF);
  toggle_max_threshold(rpm_val, RPM_MAX, rpm_up);
  toggle_min_threshold(rpm_val, RPM_MIN, rpm_down);
#if (HAS_DISPLAY)
  // Update display
  // lv_timer_handler();
  // delay(10);
#else
  Serial.print("RPM: ");
  Serial.println(buf);
#endif
}

void handle_engine_voltage(const CanFrame &rxFrame) {
  //ui_evoltage        V     0x372 0-1 battery voltage Volts y = x/10
  // ui_evoltage
  // ui_evoltageback
  // 0x372; bits 0-1 battery voltage; y = x/10
  u16 bit0 = rxFrame.data[0];
  u16 bit1 = rxFrame.data[1];
  u16 voltage_val = ((bit0 << 8) | bit1) / 10.0;
  // lv_snprintf(buf, sizeof(buf), "%d", voltage_val);
  // lv_label_set_text(ui_evoltage, buf);
  update_text(voltage_val, ui_evoltage);
  toggle_min_threshold(voltage_val, VOLTAGE_MIN, voltage);
  dim_text(voltage, ui_evoltage);
  dim_text(voltage, ui_voltagedu);
#if (HAS_DISPLAY)
  // Update display
  //lv_timer_handler();
  //delay(10);
#else
      Serial.print("Throttle: ");
  Serial.println(buf);
#endif
}
void handle_oil_pressure(const CanFrame &rxFrame) {
  //ui_eoilpressure    P     0x361 2-3 oil pressure kPa y = x/10 - 101.3
  // ui_eoilpressure
  // ui_eoilpressureback
  // 0x361; bits 2-3 Oil Pressure; y = x/10 - 101.3
  u16 bit0 = rxFrame.data[2];
  u16 bit1 = rxFrame.data[3];
  u16 pressure_val = ((bit0 << 8) | bit1) / 10.0 - 101.3;
  //lv_snprintf(buf, sizeof(buf), "%d", pressure_val);
  update_text(pressure_val, ui_eoilpressure);
  toggle_min_threshold(pressure_val, PRESSURE_MIN, pressure);
  dim_text(pressure, ui_eoilpressure);
  dim_text(pressure, ui_oilpressuredu);
#if (HAS_DISPLAY)
  // Update display
  //lv_timer_handler();
  //delay(10);
#else
  Serial.print("Oil Pressure: ");
  Serial.println(buf);
#endif
}

void handle_oil_temp(const CanFrame &rxFrame) {
  //ui_eoiltemperature T     0x3E0 6-7 oil temp     K   y = x/10
  // ui_eoiltemperature
  // ui_eoiltemperatureback
  // 0x3E0; bits 6-7; Oil temperature; y = x/10
  u16 bit0 = rxFrame.data[6];
  u16 bit1 = rxFrame.data[7];
  u16 temperature_val = (((bit0 << 8) | bit1) / 10.0) -273.15;
  //lv_snprintf(buf, sizeof(buf), "%d", temperature_val);
  update_text(temperature_val, ui_eoiltemperature);
  toggle_max_threshold(temperature_val, TEMP_MAX, temperature);
  dim_text(temperature, ui_eoiltemperature);
  dim_text(temperature, ui_oiltemperaturedu);
#if (HAS_DISPLAY)
  // Update display
  // lv_timer_handler();
  // delay(10);
#else
  Serial.print("Oil Temp: ");
  Serial.println(buf);
#endif
}

void handle_gear_selection(const CanFrame &rxFrame) {
  //ui_egear           Gear  0x470 6 gear selector position enum
  //ui_egear
  // 0x470; bits 7; gear position; enum val ??
  u16 gear_val = rxFrame.data[7];//7 gear
  //lv_snprintf(buf, sizeof(buf), "%d", gear_val);
  update_text(gear_val, ui_egear);

}

