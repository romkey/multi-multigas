#include "multi_gas.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#include <Wire.h>

namespace esphome {
namespace multi_gas {

static const char *const TAG = "multi_gas";

void MultiGas::publish_if_available(sensor::Sensor *target, bool available, float value) {
  if (target == nullptr || !available || std::isnan(value)) {
    return;
  }
  target->publish_state(value);
}

void MultiGas::setup() {
  if (!this->sensors_.begin(&Wire)) {
    ESP_LOGE(TAG, "No DFRobot gas sensors found on I2C bus");
    this->mark_failed();
    return;
  }

  this->set_timeout("warmup", 3 * 60 * 1000, [this]() {
    this->warmed_up_ = true;
    ESP_LOGI(TAG, "Warmup complete");
  });
}

void MultiGas::update() {
  if (!this->warmed_up_) {
    return;
  }

  this->publish_if_available(this->cl2_sensor_, this->sensors_.has_cl2(), this->sensors_.get_cl2());
  this->publish_if_available(this->co_sensor_, this->sensors_.has_co(), this->sensors_.get_co());
  this->publish_if_available(this->h2_sensor_, this->sensors_.has_h2(), this->sensors_.get_h2());
  this->publish_if_available(this->h2s_sensor_, this->sensors_.has_h2s(), this->sensors_.get_h2s());
  this->publish_if_available(this->hf_sensor_, this->sensors_.has_hf(), this->sensors_.get_hf());
  this->publish_if_available(this->hcl_sensor_, this->sensors_.has_hcl(), this->sensors_.get_hcl());
  this->publish_if_available(this->o2_sensor_, this->sensors_.has_o2(), this->sensors_.get_o2());
  this->publish_if_available(this->o3_sensor_, this->sensors_.has_o3(), this->sensors_.get_o3());
  this->publish_if_available(this->nh3_sensor_, this->sensors_.has_nh3(), this->sensors_.get_nh3());
  this->publish_if_available(this->no2_sensor_, this->sensors_.has_no2(), this->sensors_.get_no2());
  this->publish_if_available(this->ph3_sensor_, this->sensors_.has_ph3(), this->sensors_.get_ph3());
  this->publish_if_available(this->so2_sensor_, this->sensors_.has_so2(), this->sensors_.get_so2());
}

void MultiGas::dump_config() {
  ESP_LOGCONFIG(TAG, "MultiGas:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Warmup: 3 minutes");

  auto log_sensor = [&](const char *name, bool found, uint8_t addr) {
    if (found) {
      ESP_LOGCONFIG(TAG, "  %s: found at 0x%02X", name, addr);
    }
  };

  log_sensor("CL2", this->sensors_.has_cl2(), this->sensors_.get_cl2_i2c_addr());
  log_sensor("CO", this->sensors_.has_co(), this->sensors_.get_co_i2c_addr());
  log_sensor("H2", this->sensors_.has_h2(), this->sensors_.get_h2_i2c_addr());
  log_sensor("H2S", this->sensors_.has_h2s(), this->sensors_.get_h2s_i2c_addr());
  log_sensor("HF", this->sensors_.has_hf(), this->sensors_.get_hf_i2c_addr());
  log_sensor("HCL", this->sensors_.has_hcl(), this->sensors_.get_hcl_i2c_addr());
  log_sensor("O2", this->sensors_.has_o2(), this->sensors_.get_o2_i2c_addr());
  log_sensor("O3", this->sensors_.has_o3(), this->sensors_.get_o3_i2c_addr());
  log_sensor("NH3", this->sensors_.has_nh3(), this->sensors_.get_nh3_i2c_addr());
  log_sensor("NO2", this->sensors_.has_no2(), this->sensors_.get_no2_i2c_addr());
  log_sensor("PH3", this->sensors_.has_ph3(), this->sensors_.get_ph3_i2c_addr());
  log_sensor("SO2", this->sensors_.has_so2(), this->sensors_.get_so2_i2c_addr());
}

}  // namespace multi_gas
}  // namespace esphome
