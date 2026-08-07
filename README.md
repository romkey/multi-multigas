# MultiMultiGas

[![CI](https://github.com/romkey/multi-multigas/actions/workflows/ci.yml/badge.svg)](https://github.com/romkey/multi-multigas/actions/workflows/ci.yml)

DFRobot sells a series of I2C/UART-based gas sensors that come calibrated and are much more accurate than cheaper sensors like the analog MQ-series. These sensors can have the lower 2 bits of their I2C addresses set using switches and the middle bits set via software, so it's possible to use all of them on one I2C bus. You can query the sensor at a given address to find out what kind of gas it reports on.

This library encapsulates the [DFRobot MultiGasSensor library](https://github.com/DFRobot/DFRobot_MultiGasSensor) to provide a simple interface for querying a specific gas sensor (if available) without having to worry about gas type to I2C address mapping.

Supported gases: CL2, CO, H2, H2S, HF, HCL, O2, O3, NH3, NO2, PH3, SO2.

## Arduino / PlatformIO

Add the library and its DFRobot dependency:

```ini
lib_deps =
    https://github.com/romkey/multi-multigas
    https://github.com/DFRobot/DFRobot_MultiGasSensor
```

Basic usage:

```cpp
#include <multi-multigas.h>

MultiMultiGas sensors;

void setup() {
  Wire.begin();
  if (!sensors.begin()) {
    Serial.println("No sensors found");
    return;
  }
}

void loop() {
  if (sensors.has_co()) {
    Serial.println(sensors.get_co());
  }
}
```

Build the included example:

```bash
pio run -e example
```

Enable debug logging by defining `MULTI_MULTIGAS_DEBUG`.

### I2C address range

`begin()` scans I2C addresses **0x60–0x7F**, which is the range used by DFRobot gas sensors. Use `change_addrs()` to reassign the address group when configuring multiple sensors on one bus.

### Address planning

Each sensor type has a default address range. The lower 2 bits are set with hardware switches; the middle bits can be changed in software with `MultiMultiGas::change_addrs()`. See the DFRobot documentation for details on your specific sensor modules.

## ESPHome

Copy or reference the external component in `esphome/my_components/multi_gas/`.

```yaml
esphome:
  name: my-sensor
  libraries:
    - Wire
    - multi-multigas=https://github.com/romkey/multi-multigas

external_components:
  - source:
      type: local
      path: esphome/my_components

i2c:
  sda: GPIO21
  scl: GPIO22

multi_gas:
  id: gases
  update_interval: 60s
  co:
    name: "Carbon Monoxide"
  h2s:
    name: "Hydrogen Sulfide"
  o2:
    name: "Oxygen"
```

Notes:

- An `i2c:` block is **required** — the component scans the bus for DFRobot sensors.
- Configure only the gas sensors you want exposed; each is optional.
- O2 is reported in **percent**; other gases use **ppm**.
- Sensors need about **3 minutes** to warm up before readings are published.
- If no sensors are found at startup, the component marks itself as failed.

## API

| Method | Description |
|--------|-------------|
| `begin(wire)` | Scan 0x60–0x7F and initialize found sensors. Returns `true` if at least one sensor was found. |
| `has_co()` etc. | Returns whether that gas type was detected. |
| `get_co()` etc. | Read concentration in ppm (or `%` for O2 via DFRobot library). Returns `NaN` if not available. |
| `get_co_raw()` etc. | Read raw sensor voltage. |
| `get_co_i2c_addr()` etc. | I2C address of the detected sensor. |
| `change_addrs(start, end, group, wire)` | Change the address group for sensors in a range. |

## License

MIT
