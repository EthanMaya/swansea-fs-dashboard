#include "ui_code.hpp"
#include "touch.h"
#include "ui.h"
#include <ESP32-TWAI-CAN.hpp>

LGFX lcd;
//  SPIClass& spi = SPI;

/* Change to your screen resolution */
uint32_t screenWidth;
uint32_t screenHeight;
lv_disp_draw_buf_t draw_buf;
lv_color_t disp_draw_buf[800 * 480 / 10];
//  lv_color_t disp_draw_buf;
lv_disp_drv_t disp_drv;

char buf[16];

const uint16_t RPM_MIN = 225;
const uint16_t RPM_MAX = 675;
const uint16_t TEMP_MAX = 100;
const uint16_t PRESSURE_MIN = 60;
const uint16_t VOLTAGE_MIN = 15;

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
void update_text(uint16_t value, lv_obj_t *ui_element) {
  // Update a LVGL label
  lv_snprintf(buf, sizeof(buf), "%d", value);
  lv_label_set_text(ui_element, buf);
}

void set_dimmed_state(boolean condition, lv_obj_t *ui_element) {
  // Dim or “undim” an LVGL label based on the condition flag.
  // if condition is true, the object has full brightness otherwise it is dimmed
  if (condition) {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0xFFFFFF),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  } else {
    lv_obj_set_style_text_color(ui_element, lv_color_hex(0x555555),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  }
}
bool is_visible(lv_obj_t *o) { return !lv_obj_has_flag(o, LV_OBJ_FLAG_HIDDEN); }
void toggle_visibility(boolean alert, lv_obj_t *ui_element) {
  // No alert, hide the object
  if (!alert) {
    lv_obj_add_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
  } 
  else 
  {
    //Logic is wrong, should be if (!alert) else { ... }
    
    // Otherwise blink the object
    //update_text(is_visible(ui_element), ui_erpm);
    if (!is_visible(ui_element))
    {
      lv_obj_clear_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
      lv_obj_add_flag(ui_element, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

// Refresh speed label and arc display
void update_speed_ui(uint16_t speed_val) {
  update_text(speed_val, ui_espeed);
  lv_arc_set_value(ui_espeedarc, speed_val);
}

// Refresh RPM bar and blink alerts when thresholds crossed
void update_rpm_ui(uint16_t rpm_val) {
  update_text(rpm_val, ui_erpm);
  lv_bar_set_value(ui_erpmbar, rpm_val, LV_ANIM_OFF);
  // Blink icon when above max or below min RPM
  toggle_visibility((rpm_val > RPM_MAX), ui_erpmbackswitchup);
  toggle_visibility((rpm_val < RPM_MIN), ui_erpmbackswitchdown);
}

// Update voltage display; dim text and blink warning if below safe minimum
void update_engine_voltage_ui(uint16_t voltage_val) {
  update_text(voltage_val, ui_evoltage);
  bool dim_voltage = voltage_val < VOLTAGE_MIN; // below safe threshold
  set_dimmed_state(dim_voltage, ui_evoltage);
  set_dimmed_state(dim_voltage, ui_voltagedu);
  toggle_visibility(dim_voltage, ui_evoltageback);
}

// Update oil pressure display; dim and warn on low pressure
void update_oil_pressure_ui(uint16_t pressure_val) {
  update_text(pressure_val, ui_eoilpressure);
  bool dim_pressure = pressure_val < PRESSURE_MIN;
  set_dimmed_state(dim_pressure, ui_eoilpressure);
  set_dimmed_state(dim_pressure, ui_oilpressuredu);
  toggle_visibility(dim_pressure, ui_eoilpressureback);
}

// Update oil temperature display; dim and warn on high temperature
void update_oil_temperature_ui(uint16_t temperature_val) {
  update_text(temperature_val, ui_eoiltemperature);
  bool dim_temp = temperature_val > TEMP_MAX; // above safe threshold
  set_dimmed_state(dim_temp, ui_eoiltemperature);
  set_dimmed_state(dim_temp, ui_oiltemperaturedu);
  toggle_visibility(dim_temp, ui_eoiltemperatureback);
}

// Update gear indicator label
void update_gear_ui(uint16_t gear_val) { update_text(gear_val, ui_egear); }

// Update engine warning light; dim text and blink icon on error
void update_engine_light_ui(uint16_t engine_light_val) {
  bool error = static_cast<bool>(engine_light_val);
  set_dimmed_state(error, ui_enginedu);
  toggle_visibility(error, ui_eengineback);
}
