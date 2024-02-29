#include <Arduino.h>

#include "multi-multigas.h"

MultiMultiGas sensors;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("hello world");

  // SCL1 40, SDA1 41
  Wire1.begin(41, 40);
  sensors.begin(&Wire1);

  //  MultiMultiGas::change_addrs(0x60, 0x80, 3, &Wire1);
}

void loop() {
  Serial.print("CL2 ");
  Serial.println(sensors.get_cl2());
  Serial.print("CO ");
  Serial.println(sensors.get_co());
  Serial.print("HCL ");
  Serial.println(sensors.get_hcl());
  Serial.print("H2S ");
  Serial.println(sensors.get_h2s());
  Serial.print("NH3 ");
  Serial.println(sensors.get_nh3());
  Serial.print("NO2 ");
  Serial.println(sensors.get_no2());
  Serial.print("O3 ");
  Serial.println(sensors.get_o3());
  delay(1000);
}
