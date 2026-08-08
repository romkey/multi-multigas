#include "multi_gas.h"

#include "esphome/components/i2c/i2c_bus.h"
#if defined(USE_ESP32)
#include "esphome/components/i2c/i2c_bus_esp_idf.h"
#endif
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#include <Wire.h>

namespace esphome {
namespace multi_gas {

static const char *const TAG = "multi_gas";

static uint8_t count_detected_sensors_(const MultiMultiGas &sensors) {
  uint8_t count = 0;
  if (sensors.has_cl2()) count++;
  if (sensors.has_co()) count++;
  if (sensors.has_h2()) count++;
  if (sensors.has_h2s()) count++;
  if (sensors.has_hf()) count++;
  if (sensors.has_hcl()) count++;
  if (sensors.has_o2()) count++;
  if (sensors.has_o3()) count++;
  if (sensors.has_nh3()) count++;
  if (sensors.has_no2()) count++;
  if (sensors.has_ph3()) count++;
  if (sensors.has_so2()) count++;
  return count;
}

TwoWire *MultiGas::wire_for_bus_() {
  if (this->bus_ == nullptr) {
    ESP_LOGW(TAG, "No I2C bus configured; using default Wire");
    return &Wire;
  }

#if defined(USE_ESP32)
  // ESPHome disables RTTI (-fno-rtti), so use static_cast; bus_ is IDFI2CBus on ESP32.
  auto *idf_bus = static_cast<i2c::IDFI2CBus *>(this->bus_);
  if (idf_bus->get_port() == 1) {
    ESP_LOGCONFIG(TAG, "Using Wire1 for I2C port 1");
    return &Wire1;
  }
  ESP_LOGCONFIG(TAG, "Using Wire for I2C port %d", idf_bus->get_port());
  return &Wire;
#else
  auto *internal = static_cast<i2c::InternalI2CBus *>(this->bus_);
  ESP_LOGCONFIG(TAG, "Using Wire for I2C port %d", internal->get_port());
  return &Wire;
#endif
}

void MultiGas::debug_probe_esphome_bus_() {
  if (this->bus_ == nullptr) {
    ESP_LOGW(TAG, "ESPHome I2C bus not available for debug probe");
    return;
  }

  ESP_LOGI(TAG, "Probing ESPHome I2C bus 0x60-0x7F:");
  for (uint8_t address = 0x60; address < 0x80; address++) {
    uint8_t byte = 0;
    i2c::ErrorCode err = this->bus_->read(address, &byte, 1);
    if (err == i2c::ERROR_OK) {
      ESP_LOGI(TAG, "  ESPHome probe 0x%02X: ACK", address);
    } else if (this->debug_) {
      ESP_LOGD(TAG, "  ESPHome probe 0x%02X: err=%d", address, err);
    }
  }
}

void MultiGas::debug_log_summary_() {
  ESP_LOGI(TAG, "Configured YAML sensors:");
  auto log_cfg = [&](const char *name, sensor::Sensor *sensor) {
    if (sensor != nullptr) {
      ESP_LOGI(TAG, "  %s: configured", name);
    }
  };
  log_cfg("CL2", this->cl2_sensor_);
  log_cfg("CO", this->co_sensor_);
  log_cfg("H2", this->h2_sensor_);
  log_cfg("H2S", this->h2s_sensor_);
  log_cfg("HF", this->hf_sensor_);
  log_cfg("HCL", this->hcl_sensor_);
  log_cfg("O2", this->o2_sensor_);
  log_cfg("O3", this->o3_sensor_);
  log_cfg("NH3", this->nh3_sensor_);
  log_cfg("NO2", this->no2_sensor_);
  log_cfg("PH3", this->ph3_sensor_);
  log_cfg("SO2", this->so2_sensor_);

  ESP_LOGI(TAG, "Library detected %u sensor(s)", count_detected_sensors_(this->sensors_));
}

void MultiGas::publish_if_available(const char *name, sensor::Sensor *target, bool available, float value) {
  if (target == nullptr) {
    return;
  }
  if (!available) {
    if (this->debug_) {
      ESP_LOGD(TAG, "skip %s: not detected on bus", name);
    }
    return;
  }
  if (std::isnan(value)) {
    if (this->debug_) {
      ESP_LOGW(TAG, "skip %s: read returned NaN", name);
    }
    return;
  }
  if (this->debug_) {
    ESP_LOGD(TAG, "publish %s: %.3f", name, value);
  }
  target->publish_state(value);
}

void MultiGas::setup() {
  if (this->debug_) {
    ESP_LOGI(TAG, "Debug enabled");
    this->debug_probe_esphome_bus_();
  }

  TwoWire *wire = this->wire_for_bus_();
  if (!this->sensors_.begin(wire)) {
    ESP_LOGE(TAG, "No DFRobot gas sensors found on configured I2C bus");
    if (this->debug_) {
      ESP_LOGE(TAG, "If ESPHome i2c scan shows devices above but the library found none, check i2c_id and bus port");
      this->debug_log_summary_();
    }
    this->mark_failed();
    return;
  }

  if (this->debug_) {
    this->debug_log_summary_();
  }

  ESP_LOGI(TAG, "Found %u sensor(s); warmup 3 minutes before publishing", count_detected_sensors_(this->sensors_));

  this->set_timeout("warmup", 3 * 60 * 1000, [this]() {
    this->warmed_up_ = true;
    ESP_LOGI(TAG, "Warmup complete, starting readings");
  });
}

void MultiGas::update() {
  if (!this->warmed_up_) {
    if (this->debug_) {
      ESP_LOGD(TAG, "waiting for warmup");
    }
    return;
  }

  this->publish_if_available("CL2", this->cl2_sensor_, this->sensors_.has_cl2(), this->sensors_.get_cl2());
  this->publish_if_available("CO", this->co_sensor_, this->sensors_.has_co(), this->sensors_.get_co());
  this->publish_if_available("H2", this->h2_sensor_, this->sensors_.has_h2(), this->sensors_.get_h2());
  this->publish_if_available("H2S", this->h2s_sensor_, this->sensors_.has_h2s(), this->sensors_.get_h2s());
  this->publish_if_available("HF", this->hf_sensor_, this->sensors_.has_hf(), this->sensors_.get_hf());
  this->publish_if_available("HCL", this->hcl_sensor_, this->sensors_.has_hcl(), this->sensors_.get_hcl());
  this->publish_if_available("O2", this->o2_sensor_, this->sensors_.has_o2(), this->sensors_.get_o2());
  this->publish_if_available("O3", this->o3_sensor_, this->sensors_.has_o3(), this->sensors_.get_o3());
  this->publish_if_available("NH3", this->nh3_sensor_, this->sensors_.has_nh3(), this->sensors_.get_nh3());
  this->publish_if_available("NO2", this->no2_sensor_, this->sensors_.has_no2(), this->sensors_.get_no2());
  this->publish_if_available("PH3", this->ph3_sensor_, this->sensors_.has_ph3(), this->sensors_.get_ph3());
  this->publish_if_available("SO2", this->so2_sensor_, this->sensors_.has_so2(), this->sensors_.get_so2());
}

void MultiGas::dump_config() {
  ESP_LOGCONFIG(TAG, "MultiGas:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Debug: %s", YESNO(this->debug_));
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
