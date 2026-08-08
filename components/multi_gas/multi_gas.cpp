#include "multi_gas.h"

#include "esphome/components/i2c/i2c_bus.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace multi_gas {

static const char *const TAG = "multi_gas";

bool MultiGas::esphome_i2c_probe_(void *ctx, uint8_t address) {
  auto *self = static_cast<MultiGas *>(ctx);
  if (self->bus_ == nullptr) {
    return false;
  }
  // A zero-length transfer is address-only; this is what the bus scan uses.
  return self->bus_->write_readv(address, nullptr, 0, nullptr, 0) == i2c::ERROR_OK;
}

int MultiGas::esphome_i2c_write_(void *ctx, uint8_t address, const uint8_t *data, size_t len) {
  auto *self = static_cast<MultiGas *>(ctx);
  if (self->bus_ == nullptr) {
    return -1;
  }
  return self->bus_->write(address, data, len) == i2c::ERROR_OK ? 0 : -1;
}

int MultiGas::esphome_i2c_read_(void *ctx, uint8_t address, uint8_t *data, size_t len) {
  auto *self = static_cast<MultiGas *>(ctx);
  if (self->bus_ == nullptr) {
    return -1;
  }
  return self->bus_->read(address, data, len) == i2c::ERROR_OK ? 0 : -1;
}

void MultiGas::esphome_library_log_(void *ctx, const char *msg) {
  auto *self = static_cast<MultiGas *>(ctx);
  if (self->debug_) {
    ESP_LOGI(TAG, "%s", msg);
  }
}

void MultiGas::debug_probe_esphome_bus_() {
  if (this->bus_ == nullptr) {
    ESP_LOGW(TAG, "ESPHome I2C bus not available for debug probe");
    return;
  }

  ESP_LOGI(TAG, "Probing ESPHome I2C bus 0x60-0x7F:");
  for (uint8_t address = 0x60; address < 0x80; address++) {
    i2c::ErrorCode err = this->bus_->write_readv(address, nullptr, 0, nullptr, 0);
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

  ESP_LOGI(TAG, "Library detected %u sensor(s)", this->sensors_.sensor_count());
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
  this->sensors_.set_debug(this->debug_);
  this->sensors_.set_log_callback(esphome_library_log_, this);

  if (this->debug_) {
    ESP_LOGI(TAG, "Debug enabled");
    this->debug_probe_esphome_bus_();
  }

  MultiMultiGasI2cOps ops{};
  ops.ctx = this;
  ops.probe = esphome_i2c_probe_;
  ops.write = esphome_i2c_write_;
  ops.read = esphome_i2c_read_;

  if (!this->sensors_.begin(&ops)) {
    ESP_LOGE(TAG, "No DFRobot gas sensors found on configured I2C bus");
    if (this->debug_) {
      ESP_LOGE(TAG, "If ESPHome i2c scan shows devices above but the library found none, check i2c_id and bus wiring");
      this->debug_log_summary_();
    }
    this->mark_failed(LOG_STR("No DFRobot gas sensors found on I2C bus"));
    return;
  }

  if (this->debug_) {
    this->debug_log_summary_();
  }

  ESP_LOGI(TAG, "Found %u sensor(s); warmup 3 minutes before publishing", this->sensors_.sensor_count());

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
