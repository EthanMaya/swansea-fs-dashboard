#include "ui.h"
#include "ui_code.hpp"
#include <ESP32-TWAI-CAN.hpp>

#define CAN_TX 5
#define CAN_RX 4
#define SPEED 1000
#define HAS_DISPLAY 0
typedef u_int8_t u8;

CanFrame rxFrame;

void setup() {
  Serial.begin(9600);
#if (HAS_DISPLAY)
  init_screen();
#endif
  auto SUCCESS =
      ESP32Can.begin(ESP32Can.convertSpeed(SPEED), CAN_TX, CAN_RX, 10, 10);

  if (SUCCESS) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
}

#if (HAS_DISPLAY)
lv_obj_t **e_Throttle_Bar = &ui_Bar2;
lv_obj_t **e_Throttle_Num = &ui_Label7;
lv_obj_t **e_Gear_Position = &ui_Label8;
lv_obj_t **e_Temperature = &ui_Label5;
lv_obj_t **e_Pressure = &ui_Label6;
int y = 0;
#endif

u8 throttle = 0;
u8 temperature = 0;
u8 pressure = 0;
u8 gear = -1;

void loop() {

  if (ESP32Can.readFrame(rxFrame, 1000)) {
    throttle = rxFrame.data[0];
    temperature = rxFrame.data[1];
    gear = rxFrame.data[2];
    pressure = rxFrame.data[3];

    Serial.printf("Throttle %d \r\n", throttle);
    Serial.printf("Temperature %d \r\n", temperature);
    Serial.printf("Gear %d \r\n", gear);
    Serial.printf("Pressure %d \r\n\n", pressure);
  }

#if (HAS_DISPLAY)
  char buf[16];
  if (y >= 100) {
    lv_snprintf(buf, sizeof(buf), "%d", throttle);
    lv_label_set_text(*e_Throttle_Num, buf);
    lv_bar_set_value(*e_Throttle_Bar, throttle, LV_ANIM_OFF);
    throttle++;
    y = -1;
  }
  y++;
  lv_timer_handler(); /* let the GUI do its work */
#endif
}
