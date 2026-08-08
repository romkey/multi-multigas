#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

#include <multi-multigas.h>

#include <string>

namespace esphome {
namespace multi_gas {

class MultiGas : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_debug(bool debug) { this->debug_ = debug; }

#ifdef USE_SENSOR
  SUB_SENSOR(cl2)
  SUB_SENSOR(co)
  SUB_SENSOR(h2)
  SUB_SENSOR(h2s)
  SUB_SENSOR(hf)
  SUB_SENSOR(hcl)
  SUB_SENSOR(o2)
  SUB_SENSOR(o3)
  SUB_SENSOR(nh3)
  SUB_SENSOR(no2)
  SUB_SENSOR(ph3)
  SUB_SENSOR(so2)
#endif

#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(detected_gases)
#endif

  float get_setup_priority() const override { return setup_priority::DATA; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  void debug_probe_esphome_bus_();
  void debug_log_summary_();
  void publish_if_available(const char *name, sensor::Sensor *target, bool available, float value);
  std::string detected_gas_list_() const;

  // Drive the sensors over ESPHome's I2C bus so the component works the same
  // on Arduino and ESP-IDF, and always uses the bus selected by i2c_id.
  static bool esphome_i2c_probe_(void *ctx, uint8_t address);
  static int esphome_i2c_write_(void *ctx, uint8_t address, const uint8_t *data, size_t len);
  static int esphome_i2c_read_(void *ctx, uint8_t address, uint8_t *data, size_t len);
  static void esphome_library_log_(void *ctx, const char *msg);

  MultiMultiGas sensors_;
  bool warmed_up_{false};
  bool debug_{false};
};

}  // namespace multi_gas
}  // namespace esphome
