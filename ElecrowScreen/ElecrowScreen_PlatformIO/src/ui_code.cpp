#include "ui_code.hpp"
#include "touch.h"
#include "ui.h"
#include <ESP32-TWAI-CAN.hpp>
#include <cstdint>

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  // lcd.fillScreen(TFT_WHITE);
#if (LV_COLOR_16_SWAP != 0)
  lcd.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);
#else
  lcd.pushImageDMA(area->x1, area->y1, w, h,
                   (lgfx::rgb565_t *)&color_p->full); //
#endif

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch_has_signal()) {
    if (touch_touched()) {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
      /*Serial.print( "Data x :" );
      Serial.println( touch_last_x );

      Serial.print( "Data y :" );
      Serial.println( touch_last_y );*/

    } else if (touch_released()) {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
  delay(15);
}

void init_screen() {
  // Init Display
  lcd.begin();
  lcd.fillScreen(TFT_BLACK);
  // lcd.setTextSize(2);
  delay(200);

  lv_init();

  delay(100);
  touch_init();

  screenWidth = lcd.width();
  screenHeight = lcd.height();

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL,
                        screenWidth * screenHeight / 10);
  //  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, 480 * 272 / 10);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
  ui_init(); // 开机UI界面

  lv_timer_handler();
}

static char buf[16];
static boolean engine_error = false;
static boolean engine_switch = false;

static boolean speed = false;
static uint16_t speed_value = 0;

static boolean rpm_up = false;
static boolean rpm_up_switch = false;
static boolean rpm_down = false;
static boolean rpm_down_switch = false;
static uint16_t rpm_min = 225;
static uint16_t rpm_max = 675;
static uint16_t rpm_value = 0;

static boolean temperature = false;
static boolean temperature_switch = false;
static uint16_t temperature_max = 100;
static uint16_t temperature_value = 0;

static boolean pressure = false;
static boolean pressure_switch = false;
static uint16_t pressure_min = 60;
static uint16_t pressure_value = 0;

static boolean voltage = false;
static boolean voltage_switch = false;
static uint16_t voltage_min = 60;
static uint16_t voltage_value = 0;

static boolean gear = false;
static uint16_t gear_value = 0;

void update_text(uint16_t value, uint16_t &previous_value,
                 lv_obj_t *ui_element) {
  if (value != previous_value) {
    previous_value = value;
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(ui_element, buf);
    update = true;
  }
}
void toggle_min_threshold(uint16_t value, uint16_t min_value,
                          boolean &condition) {
  if (value < min_value) {
    condition = true;
  } else {
    condition = false;
  }
}
void toggle_max_threshold(uint16_t value, uint16_t max_value,
                          boolean &condition) {
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
}

void update_speed_ui(uint16_t speed_val) {
  //  ui_espeed
  //  ui_espeedarc
  update_text(speed_val, speed_value, ui_espeed);
  lv_arc_set_value(ui_espeedarc, speed_val);
}
void update_rpm_ui(uint16_t rpm_val) {
  //  ui_erpm
  //  ui_erpmbackswitchdown
  //  ui_erpmbackswitchup
  //  ui_erpmbar
  update_text(rpm_val, rpm_value, ui_erpm);
  lv_bar_set_value(ui_erpmbar, rpm_val, LV_ANIM_OFF);
  toggle_max_threshold(rpm_val, rpm_max, rpm_up);
  toggle_min_threshold(rpm_val, rpm_min, rpm_down);
}
void update_engine_voltage_ui(uint16_t voltage_val) {
  //  ui_evoltage
  //  ui_evoltageback
  update_text(voltage_val, voltage_value, ui_evoltage);
  toggle_min_threshold(voltage_val, voltage_min, voltage);
  dim_text(voltage, ui_evoltage);
  dim_text(voltage, ui_voltagedu);
}
void update_oil_pressure_ui(uint16_t pressure_val) {
  //  ui_eoilpressure
  //  ui_eoilpressureback
  update_text(pressure_val, pressure_value, ui_eoilpressure);
  toggle_min_threshold(pressure_val, pressure_min, pressure);
  dim_text(pressure, ui_eoilpressure);
  dim_text(pressure, ui_oilpressuredu);
}
void update_oil_temperature_ui(uint16_t temperature_val) {
  //  ui_eoiltemperature
  //  ui_eoiltemperatureback
  update_text(temperature_val, temperature_value, ui_eoiltemperature);
  toggle_max_threshold(temperature_val, temperature_max, temperature);
  dim_text(temperature, ui_eoiltemperature);
  dim_text(temperature, ui_oiltemperaturedu);
}

void update_gear_ui(uint16_t gear_val) {
  // ui_egear
  update_text(gear_val, gear_value, ui_egear);
}
void update_engine_light_ui(uint16_t engine_light_val) {
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
