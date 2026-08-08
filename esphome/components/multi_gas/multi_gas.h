#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

#include <multi-multigas.h>

namespace esphome {
namespace multi_gas {

class MultiGas : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_debug(bool debug) { this->debug_ = debug; }
  void set_sda_pin(int8_t sda_pin) { this->sda_pin_ = sda_pin; }
  void set_scl_pin(int8_t scl_pin) { this->scl_pin_ = scl_pin; }

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

  float get_setup_priority() const override { return setup_priority::DATA; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  static void log_bridge_(void *ctx, const char *msg);
  void debug_probe_esphome_bus_();
  void debug_log_summary_();
  void publish_if_available(const char *name, sensor::Sensor *target, bool available, float value);

  MultiMultiGas sensors_;
  bool warmed_up_{false};
  bool debug_{false};
  int8_t sda_pin_{-1};
  int8_t scl_pin_{-1};
};

}  // namespace multi_gas
}  // namespace esphome
