#include <lvgl.h>
#include <ESP32-TWAI-CAN.hpp>
#include "ui_code.hpp"
#include "ui.h"

void setup()
{
  Serial.begin(9600);
  init_screen();
}

lv_obj_t **e_Throttle_Bar = &ui_Bar2;
lv_obj_t **e_Throttle_Num = &ui_Label7;
lv_obj_t **e_Gear_Position = &ui_Label8;
lv_obj_t **e_Temperature = &ui_Label5;
lv_obj_t **e_Pressure = &ui_Label6;
int y = 0;

int throttle = 0;
int gear = -1;
int temperature = 0;
int pressure = 0;
void loop()
{

    char buf[16];
    if (y >= 100)
    {
      lv_snprintf(buf, sizeof(buf), "%d", throttle);
      lv_label_set_text(*e_Throttle_Num, buf);

      lv_bar_set_value(*e_Throttle_Bar, throttle, LV_ANIM_OFF);
      throttle++;

      y = -1;
    }
    y++;
    Serial.println("before GUI");

    lv_timer_handler(); /* let the GUI do its work */
    delay(10);
}