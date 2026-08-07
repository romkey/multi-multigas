#include "multi-multigas.h"

#include <cmath>

#ifdef MULTI_MULTIGAS_DEBUG
#define MULTI_MULTIGAS_LOG(...) Serial.printf(__VA_ARGS__)
#define MULTI_MULTIGAS_LOGLN(msg) Serial.println(msg)
#else
#define MULTI_MULTIGAS_LOG(...)
#define MULTI_MULTIGAS_LOGLN(msg)
#endif

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
    MULTI_MULTIGAS_LOG("replacing duplicate sensor at 0x%02x\n", slot.addr);
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
  return has_cl2() || has_co() || has_h2() || has_h2s() || has_hf() || has_hcl() || has_o2() ||
         has_o3() || has_nh3() || has_no2() || has_ph3() || has_so2();
}

bool MultiMultiGas::begin(TwoWire *wire) {
  _wire = wire;

  MULTI_MULTIGAS_LOGLN("multi-multigas: scanning I2C 0x60-0x7F");

  for (uint8_t address = MULTI_MULTIGAS_I2C_ADDR_START; address < MULTI_MULTIGAS_I2C_ADDR_END; address++) {
    MULTI_MULTIGAS_LOG(" %02x", address);
    wire->beginTransmission(address);
    if (wire->endTransmission() == 0) {
      _setup_sensor(address);
    }
  }

  MULTI_MULTIGAS_LOGLN("");
  return _any_found();
}

bool MultiMultiGas::_setup_sensor(uint8_t address) {
  DFRobot_GAS_I2C *gas = new DFRobot_GAS_I2C(_wire, address);

  MULTI_MULTIGAS_LOG("multi-multigas: found device at 0x%02x\n", address);

  if (!gas->begin()) {
    MULTI_MULTIGAS_LOGLN("multi-multigas: begin() failed");
    delete gas;
    return false;
  }

  String gas_type = gas->queryGasType();
  MULTI_MULTIGAS_LOG("multi-multigas: gas type '%s'\n", gas_type.c_str());

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
    MULTI_MULTIGAS_LOGLN("multi-multigas: unknown or empty gas type");
    delete gas;
    return false;
  }

  _assign_slot(*slot, gas, address);
  gas->changeAcquireMode(gas->PASSIVITY);
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
  MULTI_MULTIGAS_LOGLN("multi-multigas: change_addrs");

  for (uint8_t address = target_start; address < target_end; address++) {
    wire->beginTransmission(address);
    if (wire->endTransmission() == 0) {
      MULTI_MULTIGAS_LOG(" %02x", address);
      _assign_group(wire, address, group);
    }
  }

  MULTI_MULTIGAS_LOGLN("");
}

void MultiMultiGas::_assign_group(TwoWire *wire, uint8_t address, uint8_t group) {
  DFRobot_GAS_I2C gas(wire, address);
  if (!gas.begin()) {
    MULTI_MULTIGAS_LOGLN("multi-multigas: change_addrs begin() failed");
    return;
  }
  gas.changeI2cAddrGroup(group);
}
