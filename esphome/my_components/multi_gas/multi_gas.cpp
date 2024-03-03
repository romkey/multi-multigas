#include "esphome/core/log.h"
#include "multi_gas.h"

namespace esphome {
namespace multi_gas {

static const char *TAG = "multi_gas.sensor";

static MultiMultiGas sensors;

void MultiGas::setup() {
  sensors.begin();

  this->set_timeout("warmup", 3 * 60 * 1000, [this]() {
      this->warmed_up_ = true;
      ESP_LOGCONFIG(TAG, "MultiGas warmed up.");
    });

  return;
 
}

void MultiGas::loop() {
  
}

void MultiGas::update() {
  //  if (!this->warmed_up_) {
  //    return;
  //  }

  if (this->cl2_sensor_ != nullptr) {
    this->cl2_sensor_->publish_state(sensors.get_cl2());
  }

  if (this->co_sensor_ != nullptr) {
    this->co_sensor_->publish_state(sensors.get_co());
  }

  if (this->h2_sensor_ != nullptr) {
    this->h2_sensor_->publish_state(sensors.get_h2());
  }

  if (this->h2s_sensor_ != nullptr) {
    this->h2s_sensor_->publish_state(sensors.get_h2s());
  }

  if (this->hf_sensor_ != nullptr) {
    this->hf_sensor_->publish_state(sensors.get_hf());
  }

  if (this->hcl_sensor_ != nullptr) {
    this->hcl_sensor_->publish_state(sensors.get_hcl());
  }

  if (this->o2_sensor_ != nullptr) {
    this->o2_sensor_->publish_state(sensors.get_o2());
  }

  if (this->o3_sensor_ != nullptr) {
    this->o3_sensor_->publish_state(sensors.get_o3());
  }

  if (this->nh3_sensor_ != nullptr) {
    this->nh3_sensor_->publish_state(sensors.get_nh3());
  }

  if (this->no2_sensor_ != nullptr) {
    this->no2_sensor_->publish_state(sensors.get_no2());
  }

  if (this->ph3_sensor_ != nullptr) {
    this->ph3_sensor_->publish_state(sensors.get_ph3());
  }

  if (this->so2_sensor_ != nullptr) {
    this->so2_sensor_->publish_state(sensors.get_so2());
  }
}

void MultiGas::dump_config() {
    ESP_LOGCONFIG(TAG, "Empty custom sensor");
}

} //namespace multi_gas
} //namespace esphome
