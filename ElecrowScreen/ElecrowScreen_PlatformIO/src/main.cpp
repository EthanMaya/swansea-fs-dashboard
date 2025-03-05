#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <lvgl.h>

#include "ui.h"
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

#include <ESP32-TWAI-CAN.hpp>

#define TFT_BL 2
// define i2c
#define can

class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Bus_RGB _bus_instance;
  lgfx::Panel_RGB _panel_instance;

  LGFX(void) {

    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      cfg.pin_d0 = GPIO_NUM_8;  // B0
      cfg.pin_d1 = GPIO_NUM_3;  // B1
      cfg.pin_d2 = GPIO_NUM_46; // B2
      cfg.pin_d3 = GPIO_NUM_9;  // B3
      cfg.pin_d4 = GPIO_NUM_1;  // B4

      cfg.pin_d5 = GPIO_NUM_5;  // G0
      cfg.pin_d6 = GPIO_NUM_6;  // G1
      cfg.pin_d7 = GPIO_NUM_7;  // G2
      cfg.pin_d8 = GPIO_NUM_15; // G3
      cfg.pin_d9 = GPIO_NUM_16; // G4
      cfg.pin_d10 = GPIO_NUM_4; // G5

      cfg.pin_d11 = GPIO_NUM_45; // R0
      cfg.pin_d12 = GPIO_NUM_48; // R1
      cfg.pin_d13 = GPIO_NUM_47; // R2
      cfg.pin_d14 = GPIO_NUM_21; // R3
      cfg.pin_d15 = GPIO_NUM_14; // R4

      cfg.pin_henable = GPIO_NUM_40;
      cfg.pin_vsync = GPIO_NUM_41;
      cfg.pin_hsync = GPIO_NUM_39;
      cfg.pin_pclk = GPIO_NUM_0;
      cfg.freq_write = 15000000;

      cfg.hsync_polarity = 0;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 4;
      cfg.hsync_back_porch = 43;

      cfg.vsync_polarity = 0;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 4;
      cfg.vsync_back_porch = 12;

      cfg.pclk_active_neg = 1;
      cfg.de_idle_high = 0;
      cfg.pclk_idle_high = 0;

      _bus_instance.config(cfg);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = 800;
      cfg.memory_height = 480;
      cfg.panel_width = 800;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);
    setPanel(&_panel_instance);
  }
};

LGFX lcd;
SPIClass &spi = SPI;

#include "touch.h"

/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];
// static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;

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
// i2c
#if defined(i2c)
#define I2C_DEV_ADDR 0x55
#define sda 19
#define scl 20

uint32_t i = 0;

void receiveData(int byteCount) {
  Serial.println("Data received:");
  while (Wire.available()) {
    uint8_t receivedByte = Wire.read();
    Serial.print(receivedByte, HEX);
    Serial.print(" ");
  }
  Serial.println();
}
void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  Serial.println("onRequest");
  Serial.println();
}

void onReceive(int len) {
  Serial.printf("onReceive[%d]: ", len);
  while (Wire.available()) {
    Serial.write(Wire.read());
  }
  Serial.println();
}

#elif defined(can)
// Default for ESP32
#define CAN_TX 19
#define CAN_RX 20
CanFrame rxFrame;

void sendObdFrame(uint8_t obdId) {
  CanFrame obdFrame = {0};
  obdFrame.identifier = 0x7DF; // Default OBD2 address;
  obdFrame.extd = 0;
  obdFrame.data_length_code = 8;
  obdFrame.data[0] = 2;
  obdFrame.data[1] = 1;
  obdFrame.data[2] = obdId;
  obdFrame.data[3] = 0xAA; // Best to use 0xAA (0b10101010) instead of 0
  obdFrame.data[4] = 0xAA; // CAN works better this way as it needs
  obdFrame.data[5] = 0xAA; // to avoid bit-stuffing
  obdFrame.data[6] = 0xAA;
  obdFrame.data[7] = 0xAA;
  // Accepts both pointers and references
  ESP32Can.writeFrame(obdFrame); // timeout defaults to 1 ms
}
#endif

void init_screen() {
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
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, 480 * 272 / 10);

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

void init_can() {
  ESP32Can.setPins(CAN_TX, CAN_RX);

  // You can set custom size for the queues - those are default
  ESP32Can.setRxQueueSize(5);
  ESP32Can.setTxQueueSize(5);

  // .setSpeed() and .begin() functions require to use TwaiSpeed enum,
  // but you can easily convert it from numerical value using .convertSpeed()
  ESP32Can.setSpeed(ESP32Can.convertSpeed(500));

  // You can also just use .begin()..
  if (ESP32Can.begin()) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }

  // or override everything in one command;
  // It is also safe to use .begin() without .end() as it calls it internally
  if (ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
}
void setup() {
  Serial.begin(9600);
#ifdef ESP32SCREEN
  init_screem();
#endif
  Serial.println("Setup done");
#if defined(i2c)
  Serial.setDebugOutput(true);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  // Wire.begin((uint8_t)I2C_DEV_ADDR);
  Wire.begin((uint8_t)I2C_DEV_ADDR, sda, scl, 100000);
  // Screen
  //  Serial.println("LVGL Widgets Demo");
  // Wire.begin(19, 20);
  // IO Pin
  pinMode(38, INPUT);
  // digitalWrite(38, LOW);

#elif defined(can)
  init_can();
#endif
  Serial.println("Can done");
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
void loop() {
  Serial.println("before can");
#if defined(can)
  static uint32_t lastStamp = 0;
  uint32_t currentStamp = millis();

  /*
  if(currentStamp - lastStamp > 1000) {   // sends OBD2 request every second
      lastStamp = currentStamp;
      sendObdFrame(5); // For coolant temperature
  }
  */

  // You can set custom timeout, default is 1000
  if (ESP32Can.readFrame(rxFrame, 10)) {
    // Comment out if too many frames
    Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
    if (rxFrame.identifier == 0x7E8) { // Standard OBD2 frame responce ID
      Serial.printf("Collant temp: %3d°C \r\n",
                    rxFrame.data[3] - 40); // Convert to °C
    }
  }
  Serial.println("in can");
#endif
  Serial.println("behind can");

  delay(10);
  // char buf[16];
  // if (y >= 100) {
  //   lv_snprintf(buf, sizeof(buf), "%d", throttle);
  //   lv_label_set_text(*e_Throttle_Num, buf);
  //
  //   lv_bar_set_value(*e_Throttle_Bar, throttle, LV_ANIM_OFF);
  //   throttle++;
  //
  //   y = -1;
  // }
  // y++;
  // Serial.println("before GUI");
  //
  // lv_timer_handler(); /* let the GUI do its work */
  Serial.println("After GUI");
  delay(10);
}
