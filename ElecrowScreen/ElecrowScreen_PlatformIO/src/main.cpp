#include "can_rule_engine.h"
#include "driver/twai.h"
#include "ui.h"
#include "ui_code.hpp"
#include <ESP32-TWAI-CAN.hpp>
#include <lvgl.h>
#include "config.h"

#define CAN_TX 44
#define CAN_RX 43
#define SPEED 1000
#define HAS_DISPLAY 1

typedef uint8_t u8;
typedef uint16_t u16;

char buf[16];
CanFrame rxFrame;
RuleEngine<CanFrame> rule_engine;
volatile bool update = false;


class CompareIdentifier {
  u16 identifier;
public:
  CompareIdentifier(u16 identifier) : identifier(identifier) {}
  bool operator()(const CanFrame &rxFrame) {
    return rxFrame.identifier == identifier;
  }
};

void handle_gear_selection(const CanFrame &rxFrame);
void dim_text(bool condition, lv_obj_t *ui_element);
void update_text(u16 value, lv_obj_t *ui_element);

TaskHandle_t TaskCanReader;
TaskHandle_t TaskDisplayUpdater;

// Task: Read CAN and run rule engine
void can_read_task(void *parameter) {
  while (true) {
    if (ESP32Can.readFrame(rxFrame, 1000)) {
      rule_engine.run(rxFrame);
    }
    vTaskDelay(1); // Yield
  }
}

// Task: Update display when needed
void display_task(void *parameter) {
  while (true) {
    if (update) {
      lv_timer_handler();  // Refresh the LVGL UI
      update = false;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Run at ~100 Hz
  }
}

void setup() {
  Serial.begin(9600);

  bool SUCCESS = ESP32Can.begin(ESP32Can.convertSpeed(SPEED), CAN_TX, CAN_RX, 10, 10);
  if (SUCCESS) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }

  rule_engine.add_rule(CompareIdentifier(0x470), &handle_gear_selection);

#if (HAS_DISPLAY)
  init_screen();
#endif

  // Start CAN reader on Core 0
  xTaskCreatePinnedToCore(
    can_read_task,          // Task function
    "CAN Reader",           // Name
    4096,                   // Stack size
    NULL,                   // Param
    1,                      // Priority
    &TaskCanReader,         // Task handle
    0                       // Core 0
  );

  // Start display update on Core 1
  xTaskCreatePinnedToCore(
    display_task,
    "Display Updater",
    4096,
    NULL,
    1,
    &TaskDisplayUpdater,
    1 // Core 1
  );
}

void loop() {
  // Empty, tasks handle everything
}

// ====================== UI helpers ===========================



void dim_text(bool condition, lv_obj_t *ui_element) {
  lv_color_t color = condition ? lv_color_hex(0xFFFFFF) : lv_color_hex(0x555555);
  lv_obj_set_style_text_color(ui_element, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void update_text(u16 value, lv_obj_t *ui_element) {
  lv_snprintf(buf, sizeof(buf), "%d", value);
  lv_label_set_text(ui_element, buf);
  update = true;
}

void handle_gear_selection(const CanFrame &rxFrame) {
  u16 gear_val = rxFrame.data[7];
  update_text(gear_val, ui_egear);
}
