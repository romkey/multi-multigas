#include "multi-multigas.h"

#include <cmath>
#include <cstdarg>
#include <cstdio>

#ifdef MULTI_MULTIGAS_DEBUG
#define MULTI_MULTIGAS_SERIAL_LOG(...) Serial.printf(__VA_ARGS__)
#else
#define MULTI_MULTIGAS_SERIAL_LOG(...)
#endif

void MultiMultiGas::_log(const char *fmt, ...) const {
  char buffer[192];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  if (this->_log_callback != nullptr) {
    this->_log_callback(this->_log_callback_ctx, buffer);
  }
  if (this->_debug) {
    MULTI_MULTIGAS_SERIAL_LOG("%s\n", buffer);
  }
}

void MultiMultiGas::set_log_callback(LogCallback cb, void *ctx) {
  this->_log_callback = cb;
  this->_log_callback_ctx = ctx;
}

uint8_t MultiMultiGas::_count_sensors() const {
  uint8_t count = 0;
  if (has_cl2()) count++;
  if (has_co()) count++;
  if (has_h2()) count++;
  if (has_h2s()) count++;
  if (has_hf()) count++;
  if (has_hcl()) count++;
  if (has_o2()) count++;
  if (has_o3()) count++;
  if (has_nh3()) count++;
  if (has_no2()) count++;
  if (has_ph3()) count++;
  if (has_so2()) count++;
  return count;
}

uint8_t MultiMultiGas::sensor_count() const { return _count_sensors(); }

MultiMultiGas::MultiMultiGas() = default;

MultiMultiGas::~MultiMultiGas() {
  _clear_slot(_cl2);
  _clear_slot(_co);
  _clear_slot(_h2);
  _clear_slot(_h2s);
  _clear_slot(_hf);
  _clear_slot(_hcl);
  _clear_slot(_o2);
  _clear_slot(_o3);
  _clear_slot(_nh3);
  _clear_slot(_no2);
  _clear_slot(_ph3);
  _clear_slot(_so2);
}

void MultiMultiGas::_clear_slot(Slot &slot) {
  delete slot.sensor;
  slot.sensor = nullptr;
  slot.addr = 0;
}

void MultiMultiGas::_assign_slot(Slot &slot, DFRobot_GAS_I2C *gas, uint8_t address) {
  if (slot.sensor != nullptr) {
    _log("replacing duplicate sensor at 0x%02x", slot.addr);
    _clear_slot(slot);
  }
  slot.sensor = gas;
  slot.addr = address;
}

float MultiMultiGas::_read_ppm(const Slot &slot) {
  if (slot.sensor != nullptr) {
    return slot.sensor->readGasConcentrationPPM();
  }
  return NAN;
}

float MultiMultiGas::_read_raw(const Slot &slot) {
  if (slot.sensor != nullptr) {
    return slot.sensor->getSensorVoltage();
  }
  return NAN;
}

bool MultiMultiGas::_any_found() const {
  return _count_sensors() > 0;
}

bool MultiMultiGas::begin(TwoWire *wire) {
  _wire = wire;

  _log("scanning I2C 0x%02X-0x%02X", MULTI_MULTIGAS_I2C_ADDR_START, MULTI_MULTIGAS_I2C_ADDR_END - 1);

  for (uint8_t address = MULTI_MULTIGAS_I2C_ADDR_START; address < MULTI_MULTIGAS_I2C_ADDR_END; address++) {
    wire->beginTransmission(address);
    uint8_t err = wire->endTransmission();
    if (err == 0) {
      _log("Wire probe 0x%02X: ACK", address);
      _setup_sensor(address);
    } else if (this->_debug) {
      _log("Wire probe 0x%02X: no response (err=%u)", address, err);
    }
  }

  _log("scan complete: %u sensor(s) initialized", _count_sensors());
  return _any_found();
}

bool MultiMultiGas::_setup_sensor(uint8_t address) {
  DFRobot_GAS_I2C *gas = new DFRobot_GAS_I2C(_wire, address);

  _log("initializing DFRobot sensor at 0x%02X", address);

  if (!gas->begin()) {
    _log("DFRobot begin() failed at 0x%02X", address);
    delete gas;
    return false;
  }

  String gas_type = gas->queryGasType();
  _log("0x%02X reports gas type '%s' (len=%u)", address, gas_type.c_str(), gas_type.length());

  Slot *slot = nullptr;

  if (gas_type == "CL2") {
    slot = &_cl2;
  } else if (gas_type == "CO") {
    slot = &_co;
  } else if (gas_type == "H2") {
    slot = &_h2;
  } else if (gas_type == "H2S") {
    slot = &_h2s;
  } else if (gas_type == "HF") {
    slot = &_hf;
  } else if (gas_type == "HCL") {
    slot = &_hcl;
  } else if (gas_type == "O2") {
    slot = &_o2;
  } else if (gas_type == "O3") {
    slot = &_o3;
  } else if (gas_type == "NH3") {
    slot = &_nh3;
  } else if (gas_type == "NO2") {
    slot = &_no2;
  } else if (gas_type == "PH3") {
    slot = &_ph3;
  } else if (gas_type == "SO2") {
    slot = &_so2;
  }

  if (slot == nullptr) {
    _log("unsupported or empty gas type at 0x%02X: '%s'", address, gas_type.c_str());
    delete gas;
    return false;
  }

  _assign_slot(*slot, gas, address);
  gas->changeAcquireMode(gas->PASSIVITY);
  _log("registered %s at 0x%02X", gas_type.c_str(), address);
  return true;
}

float MultiMultiGas::get_cl2() { return _read_ppm(_cl2); }
float MultiMultiGas::get_co() { return _read_ppm(_co); }
float MultiMultiGas::get_h2() { return _read_ppm(_h2); }
float MultiMultiGas::get_h2s() { return _read_ppm(_h2s); }
float MultiMultiGas::get_hf() { return _read_ppm(_hf); }
float MultiMultiGas::get_hcl() { return _read_ppm(_hcl); }
float MultiMultiGas::get_o2() { return _read_ppm(_o2); }
float MultiMultiGas::get_o3() { return _read_ppm(_o3); }
float MultiMultiGas::get_nh3() { return _read_ppm(_nh3); }
float MultiMultiGas::get_no2() { return _read_ppm(_no2); }
float MultiMultiGas::get_ph3() { return _read_ppm(_ph3); }
float MultiMultiGas::get_so2() { return _read_ppm(_so2); }

float MultiMultiGas::get_cl2_raw() { return _read_raw(_cl2); }
float MultiMultiGas::get_co_raw() { return _read_raw(_co); }
float MultiMultiGas::get_h2_raw() { return _read_raw(_h2); }
float MultiMultiGas::get_h2s_raw() { return _read_raw(_h2s); }
float MultiMultiGas::get_hf_raw() { return _read_raw(_hf); }
float MultiMultiGas::get_hcl_raw() { return _read_raw(_hcl); }
float MultiMultiGas::get_o2_raw() { return _read_raw(_o2); }
float MultiMultiGas::get_o3_raw() { return _read_raw(_o3); }
float MultiMultiGas::get_nh3_raw() { return _read_raw(_nh3); }
float MultiMultiGas::get_no2_raw() { return _read_raw(_no2); }
float MultiMultiGas::get_ph3_raw() { return _read_raw(_ph3); }
float MultiMultiGas::get_so2_raw() { return _read_raw(_so2); }

void MultiMultiGas::change_addrs(uint8_t target_start, uint8_t target_end, uint8_t group, TwoWire *wire) {
  MULTI_MULTIGAS_SERIAL_LOG("multi-multigas: change_addrs 0x%02X-0x%02X to group %u\n", target_start,
                            target_end - 1, group);

  for (uint8_t address = target_start; address < target_end; address++) {
    wire->beginTransmission(address);
    if (wire->endTransmission() == 0) {
      MULTI_MULTIGAS_SERIAL_LOG("multi-multigas: reassigning 0x%02X\n", address);
      _assign_group(wire, address, group);
    }
  }
}

void MultiMultiGas::_assign_group(TwoWire *wire, uint8_t address, uint8_t group) {
  DFRobot_GAS_I2C gas(wire, address);
  if (!gas.begin()) {
    MULTI_MULTIGAS_SERIAL_LOG("multi-multigas: change_addrs begin() failed at 0x%02X\n", address);
    return;
  }
  gas.changeI2cAddrGroup(group);
}
