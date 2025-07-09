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


void display_update();

class CompareIdentifier {
  u16 identifier;

public:
  CompareIdentifier(u16 identifier) : identifier(identifier){};
  bool operator()(const CanFrame &rxFrame) {
    return rxFrame.identifier == identifier;
  }
};

void handle_gear_selection(const CanFrame &rxFrame);

//
char buf[16];
CanFrame rxFrame;
RuleEngine<CanFrame> rule_engine;


boolean update = false;


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

#if (HAS_DISPLAY)
  init_screen();
#endif
}

void loop() 
{
  update = false;
  if (ESP32Can.readFrame(rxFrame, 1000))
  {
    //Serial.println("In loop");
    rule_engine.run(rxFrame);
  }
  if (update)
  {
    lv_timer_handler();
    //delay(10);
  }
}

void dim_text(boolean condition, lv_obj_t *ui_element) 
{
  if (condition) 
  {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
  }
  else
  {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
  }

}

void update_text(u16 value, lv_obj_t *ui_element) 
{
  lv_snprintf(buf, sizeof(buf), "%d", value);
  lv_label_set_text(ui_element, buf);
  update = true;
}

void handle_gear_selection(const CanFrame &rxFrame) 
{
  //ui_egear           Gear  0x470 6 gear selector position enum
  //ui_egear
  // 0x470; bits 7; gear position; enum val ??
  u16 gear_val = rxFrame.data[7];//7 gear
  //lv_snprintf(buf, sizeof(buf), "%d", gear_val);
  update_text(gear_val, ui_egear);

}

