#include "multi-multigas.h"

bool MultiMultiGas::begin(TwoWire *wire) {
  Serial.println("begin");
  _wire = wire;

  Serial.println("scanning I2C");

  for(uint8_t address = 0x60; address < 0x80; address++ ) {
    Serial.printf(" %02x", address);
    wire->beginTransmission(address);
    if (wire->endTransmission() == 0)
      _setup_sensor(address);
  }

  return true;
}

bool MultiMultiGas::_setup_sensor(uint8_t address) {
  DFRobot_GAS_I2C *gas = new DFRobot_GAS_I2C(_wire, address);

  Serial.printf("found sensor at %02x\n", address);

  if(!gas->begin()) {
    Serial.println("nope");
    return false;
  }

  String gas_type = gas->queryGasType();
  Serial.println(gas_type);

  if(gas_type == "CL2") {
    _cl2_addr = address;
    _cl2_sensor = gas;
  }

  if(gas_type == "CO") {
    _co_addr = address;
    _co_sensor = gas;
  }

  if(gas_type == "H2") {
    _h2_addr = address;
    _h2_sensor = gas;
  }

  if(gas_type == "H2S") {
    _h2s_addr = address;
    _h2s_sensor = gas;
  }

  if(gas_type == "HF") {
    _hf_addr = address;
    _hf_sensor = gas;
  }

  if(gas_type == "HCL") {
    _hcl_addr = address;
    _hcl_sensor = gas;
  }

  if(gas_type == "O2") {
    _o2_addr = address;
    _o2_sensor = gas;
  }

  if(gas_type == "O3") {
    _o3_addr = address;
    _o3_sensor = gas;
  }

  if(gas_type == "NH3") {
    _nh3_addr = address;
    _nh3_sensor = gas;
  }

  if(gas_type == "NO2") {
    _no2_addr = address;
    _no2_sensor = gas;
  }

  if(gas_type == "PH3") {
    _ph3_addr = address;
    _ph3_sensor = gas;
  }

  if(gas_type == "SO2") {
    _so2_addr = address;
    _so2_sensor = gas;
  }

  if(gas_type == "")
    return false;

  gas->changeAcquireMode(gas->PASSIVITY);
  return true;
}


float MultiMultiGas::get_cl2() {
  if(this->_cl2_sensor)
    return this->_cl2_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_co() {
  if(this->_co_sensor)
    return this->_co_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_h2() {
  if(this->_h2_sensor)
    return this->_h2_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_h2s() {
  if(this->_h2s_sensor)
    return this->_h2s_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_hf() {
  if(this->_hf_sensor)
    return this->_hf_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_hcl() {
  if(this->_hcl_sensor)
    return this->_hcl_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_o2() {
  if(this->_o2_sensor)
    return this->_o2_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_o3() {
  if(this->_o3_sensor)
    return this->_o3_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_nh3() {
  if(this->_nh3_sensor)
    return this->_nh3_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_no2() {
  if(this->_no2_sensor)
    return this->_no2_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_ph3() {
  if(this->_ph3_sensor)
    return this->_ph3_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}

float MultiMultiGas::get_so2() {
  if(this->_so2_sensor)
    return this->_so2_sensor->readGasConcentrationPPM();
  else
    return nanf("");
}


float MultiMultiGas::get_cl2_raw() {
  if(this->_cl2_sensor)
    return this->_cl2_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_co_raw() {
  if(this->_co_sensor)
    return this->_co_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_h2_raw() {
  if(this->_h2_sensor)
    return this->_h2_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_h2s_raw() {
  if(this->_h2s_sensor)
    return this->_h2s_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_hf_raw() {
  if(this->_hf_sensor)
    return this->_hf_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_hcl_raw() {
  if(this->_hcl_sensor)
    return this->_hcl_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_o2_raw() {
  if(this->_o2_sensor)
    return this->_o2_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_o3_raw() {
  if(this->_o3_sensor)
    return this->_o3_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_nh3_raw() {
  if(this->_nh3_sensor)
    return this->_nh3_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_no2_raw() {
  if(this->_no2_sensor)
    return this->_no2_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_ph3_raw() {
  if(this->_ph3_sensor)
    return this->_ph3_sensor->getSensorVoltage();
  else
    return nanf("");
}

float MultiMultiGas::get_so2_raw() {
  if(this->_so2_sensor)
    return this->_so2_sensor->getSensorVoltage();
  else
    return nanf("");
}



//void MultiMultiGas::change_addrs(uint8_t target_start, uint8_t target_end, uint8_t group, TwoWire &wire = Wire) {
void MultiMultiGas::change_addrs(uint8_t target_start, uint8_t target_end, uint8_t group, TwoWire *wire) {
  Serial.println("change_addrs begin");

  for(uint8_t address = target_start; address < target_end; address++ ) {
    wire->beginTransmission(address);
    if (wire->endTransmission() == 0) {
      Serial.printf(" %02x", address);
      _assign_group(wire, address, group);
    }
  }
}

void MultiMultiGas::_assign_group(TwoWire *wire, uint8_t address, uint8_t group) {
  DFRobot_GAS_I2C gas(wire, address);
  if(!gas.begin()) {
    Serial.println("fail");
    return;
  }
  gas.changeI2cAddrGroup(group);
}
