#include "can_rules.hpp"
#include "ui_code.hpp"

void handle_speed(const CanFrame &rxFrame) {
  //  0x370; bits 0-1 vehicle speed km/h y = x/10
  uint16_t bit0 = rxFrame.data[0];
  uint16_t bit1 = rxFrame.data[1];
  uint16_t speed_val = ((bit0 << 8) | bit1) / 10;
#if (HAS_DISPLAY)
  update_speed_ui(speed_val);
#else
#endif
}

void handle_rpm(const CanFrame &rxFrame) {
  // 0x360; bits 0-1 RPM; y = x
  uint16_t bit0 = rxFrame.data[0];
  uint16_t bit1 = rxFrame.data[1];
  uint16_t rpm_val = ((bit0 << 8) | bit1);
#if (HAS_DISPLAY)
  update_rpm_ui(rpm_val);
#else
#endif
}

void handle_oil_pressure(const CanFrame &rxFrame) {
  //  0x361; bits 2-3 Oil Pressure KPa; y = x/10 - 101.3
  uint16_t bit0 = rxFrame.data[2];
  uint16_t bit1 = rxFrame.data[3];
  uint16_t pressure_val = ((bit0 << 8) | bit1) / 10.0 - 101.3;
#if (HAS_DISPLAY)
  update_oil_pressure_ui(pressure_val);
#else
#endif
}

void handle_engine_voltage(const CanFrame &rxFrame) {
  //  0x372; bits 0-1 battery voltage Volts; y = x/10
  uint16_t bit0 = rxFrame.data[0];
  uint16_t bit1 = rxFrame.data[1];
  uint16_t voltage_val = ((bit0 << 8) | bit1) / 10.0;
#if (HAS_DISPLAY)
  update_engine_voltage_ui(voltage_val);
#else
#endif
}

void handle_oil_temp(const CanFrame &rxFrame) {
  //  0x3E0; bits 6-7; Oil temperature K; y = x/10
  uint16_t bit0 = rxFrame.data[6];
  uint16_t bit1 = rxFrame.data[7];
  uint16_t temperature_val = ((bit0 << 8) | bit1) / 10.0;
#if (HAS_DISPLAY)
  update_oil_temperature_ui(temperature_val);
#else
#endif
}
void handle_gear_selection(const CanFrame &rxFrame) {
  //  0x470; bits 7; gear position, enum; enum val ??
  uint16_t gear_val = rxFrame.data[6];
#if (HAS_DISPLAY)
  update_gear_ui(gear_val);
#else
#endif
}

void handle_engine_light(const CanFrame &rxFrame) {
  //  0x3E4 bits 7:7 check engine light boolean 0=off, 1=on
  uint16_t engine_light_val = (rxFrame.data[7] >> 7) & 0x01;
#if (HAS_DISPLAY)
  update_engine_light_ui(engine_light_val);
#else
#endif
}
