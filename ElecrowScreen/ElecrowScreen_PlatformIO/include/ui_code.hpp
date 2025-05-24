#ifndef _UI_CODE_HPP
#define _UI_CODE_HPP

#include <lvgl.h>
// #include <SPI.h>
// #include <Wire.h>
#include "lgfx.hpp"

#define TFT_BL 2
// define i2c
// #define can

extern LGFX lcd;
// extern SPIClass& spi = SPI;

/* Change to your screen resolution */
extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern lv_disp_draw_buf_t draw_buf;

extern lv_color_t disp_draw_buf[800 * 480 / 10];
// extern lv_color_t disp_draw_buf;
extern lv_disp_drv_t disp_drv;

extern boolean update;

extern char buf[16];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void init_screen();
void update_speed_ui(uint16_t speed_val);
void update_rpm_ui(uint16_t rpm_val);
void update_engine_voltage_ui(uint16_t voltage_val);
void update_oil_pressure_ui(uint16_t pressure_val);
void update_oil_temperature_ui(uint16_t temperature_val);
void update_gear_ui(uint16_t gear_val);
void update_engine_light_ui(uint16_t engine_light_val);

#endif
