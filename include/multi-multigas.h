#pragma once

#include <Arduino.h>

#include <DFRobot_MultiGasSensor.h>
#include <Wire.h>

// DFRobot gas sensors use I2C addresses in the range 0x60-0x7F.
static constexpr uint8_t MULTI_MULTIGAS_I2C_ADDR_START = 0x60;
static constexpr uint8_t MULTI_MULTIGAS_I2C_ADDR_END = 0x80;

// Transport hooks so the sensors can be driven over a non-Arduino I2C stack
// (e.g. ESPHome's ESP-IDF bus). Reads must be a separate transaction from the
// preceding register write: the DFRobot firmware expects a STOP between them
// rather than a repeated start.
struct MultiMultiGasI2cOps {
  void *ctx = nullptr;
  bool (*probe)(void *ctx, uint8_t address) = nullptr;
  int (*write)(void *ctx, uint8_t address, const uint8_t *data, size_t len) = nullptr;
  int (*read)(void *ctx, uint8_t address, uint8_t *data, size_t len) = nullptr;
};

class MultiMultiGas {
 public:
  MultiMultiGas();
  ~MultiMultiGas();

  // Scans 0x60-0x7F for DFRobot gas sensors. Returns true if at least one
  // sensor was found and initialized.
  bool begin(TwoWire *wire = &Wire);
  bool begin(const MultiMultiGasI2cOps *ops);

  using LogCallback = void (*)(void *ctx, const char *msg);
  void set_log_callback(LogCallback cb, void *ctx = nullptr);
  void set_debug(bool enable) { _debug = enable; }
  bool debug() const { return _debug; }
  uint8_t sensor_count() const;

  TwoWire *wire() const { return _wire; }

  bool has_cl2() const { return _cl2.sensor != nullptr; }
  bool has_co() const { return _co.sensor != nullptr; }
  bool has_h2() const { return _h2.sensor != nullptr; }
  bool has_h2s() const { return _h2s.sensor != nullptr; }
  bool has_hcl() const { return _hcl.sensor != nullptr; }
  bool has_hf() const { return _hf.sensor != nullptr; }
  bool has_o2() const { return _o2.sensor != nullptr; }
  bool has_o3() const { return _o3.sensor != nullptr; }
  bool has_nh3() const { return _nh3.sensor != nullptr; }
  bool has_no2() const { return _no2.sensor != nullptr; }
  bool has_ph3() const { return _ph3.sensor != nullptr; }
  bool has_so2() const { return _so2.sensor != nullptr; }

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

  float get_cl2_raw();
  float get_co_raw();
  float get_h2_raw();
  float get_h2s_raw();
  float get_hcl_raw();
  float get_hf_raw();
  float get_o2_raw();
  float get_o3_raw();
  float get_nh3_raw();
  float get_no2_raw();
  float get_ph3_raw();
  float get_so2_raw();

  uint8_t get_cl2_i2c_addr() const { return _cl2.addr; }
  uint8_t get_co_i2c_addr() const { return _co.addr; }
  uint8_t get_h2_i2c_addr() const { return _h2.addr; }
  uint8_t get_h2s_i2c_addr() const { return _h2s.addr; }
  uint8_t get_hcl_i2c_addr() const { return _hcl.addr; }
  uint8_t get_hf_i2c_addr() const { return _hf.addr; }
  uint8_t get_o2_i2c_addr() const { return _o2.addr; }
  uint8_t get_o3_i2c_addr() const { return _o3.addr; }
  uint8_t get_nh3_i2c_addr() const { return _nh3.addr; }
  uint8_t get_no2_i2c_addr() const { return _no2.addr; }
  uint8_t get_ph3_i2c_addr() const { return _ph3.addr; }
  uint8_t get_so2_i2c_addr() const { return _so2.addr; }

  static void change_addrs(uint8_t target_start, uint8_t target_end, uint8_t group, TwoWire *wire = &Wire);

 private:
  struct Slot {
    uint8_t addr = 0;
    DFRobot_GAS *sensor = nullptr;
  };

  bool _begin_scan_();
  bool _setup_sensor(uint8_t address);
  void _assign_slot(Slot &slot, DFRobot_GAS *gas, uint8_t address);
  void _clear_slot(Slot &slot);
  void _log(const char *fmt, ...) const;
  static float _read_ppm(const Slot &slot);
  static float _read_raw(const Slot &slot);
  bool _any_found() const;
  uint8_t _count_sensors() const;

  static void _assign_group(TwoWire *wire, uint8_t address, uint8_t group);

  TwoWire *_wire = nullptr;
  MultiMultiGasI2cOps _i2c_ops_{};
  LogCallback _log_callback = nullptr;
  void *_log_callback_ctx = nullptr;
  bool _debug = false;

  Slot _cl2;
  Slot _co;
  Slot _h2;
  Slot _h2s;
  Slot _hf;
  Slot _hcl;
  Slot _o2;
  Slot _o3;
  Slot _nh3;
  Slot _no2;
  Slot _ph3;
  Slot _so2;
};
