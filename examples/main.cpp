#include <Arduino.h>

#include <multi-multigas.h>

MultiMultiGas sensors;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("multi-multigas example");

  // SCL1 40, SDA1 41
  Wire1.begin(41, 40);
  if (!sensors.begin(&Wire1)) {
    Serial.println("No gas sensors found");
    return;
  }

  // Reassign address group for sensors in range 0x60-0x80 to group 3:
  // MultiMultiGas::change_addrs(0x60, 0x80, 3, &Wire1);
}

void loop() {
  if (sensors.has_cl2()) {
    Serial.print("CL2 ");
    Serial.println(sensors.get_cl2());
  }
  if (sensors.has_co()) {
    Serial.print("CO ");
    Serial.println(sensors.get_co());
  }
  if (sensors.has_hcl()) {
    Serial.print("HCL ");
    Serial.println(sensors.get_hcl());
  }
  if (sensors.has_h2s()) {
    Serial.print("H2S ");
    Serial.println(sensors.get_h2s());
  }
  if (sensors.has_nh3()) {
    Serial.print("NH3 ");
    Serial.println(sensors.get_nh3());
  }
  if (sensors.has_no2()) {
    Serial.print("NO2 ");
    Serial.println(sensors.get_no2());
  }
  if (sensors.has_o3()) {
    Serial.print("O3 ");
    Serial.println(sensors.get_o3());
  }
  delay(1000);
}
