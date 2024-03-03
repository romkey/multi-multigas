#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include <multi-multigas.h>

namespace esphome {
namespace multi_gas {

class MultiGas : public sensor::Sensor, public PollingComponent {
  public:
  MultiGas() = default;

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

    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;

 protected:
    bool warmed_up_{false};
};

} //namespace multi_gas
} //namespace esphome
