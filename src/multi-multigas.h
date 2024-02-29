#pragma once

#include <Arduino.h>

#include <DFRobot_MultiGasSensor.h>
#include <Wire.h>

class MultiMultiGas {
 public:
  bool begin(TwoWire *wire = &Wire);

  float get_cl2();
  float get_co();
  float get_h2();
  float get_h2s();
  float get_hcl();
  float get_hf();
  float get_o2();
  float get_o3();
  float get_nh3();
  float get_no2();
  float get_ph3();
  float get_so2();

  static void change_addrs(uint8_t target_start, uint8_t target_end, uint8_t new_range, TwoWire *wire = &Wire);

 private:
  bool _setup_sensor(uint8_t address);
  static void _assign_group(TwoWire *wire, uint8_t address, uint8_t group);

  TwoWire *_wire;

  uint8_t _cl2_addr;
  DFRobot_GAS_I2C *_cl2_sensor;
  uint8_t _co_addr;
  DFRobot_GAS_I2C *_co_sensor;
  uint8_t _h2_addr;
  DFRobot_GAS_I2C *_h2_sensor;
  uint8_t _h2s_addr;
  DFRobot_GAS_I2C *_h2s_sensor;
  uint8_t _hf_addr;
  DFRobot_GAS_I2C *_hf_sensor;
  uint8_t _hcl_addr;
  DFRobot_GAS_I2C *_hcl_sensor;
  uint8_t _o2_addr;
  DFRobot_GAS_I2C *_o2_sensor;
  uint8_t _o3_addr;
  DFRobot_GAS_I2C *_o3_sensor;
  uint8_t _nh3_addr;
  DFRobot_GAS_I2C *_nh3_sensor;
  uint8_t _no2_addr;
  DFRobot_GAS_I2C *_no2_sensor;
  uint8_t _ph3_addr;
  DFRobot_GAS_I2C *_ph3_sensor;
  uint8_t _so2_addr;
  DFRobot_GAS_I2C *_so2_sensor;
};

