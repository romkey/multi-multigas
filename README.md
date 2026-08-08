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

Each sensor type has a default address range. The lower 2 bits are set with hardware switches; the middle bits can be changed in software with `MultiMultiGas::change_addrs()`. DFRobot divides the range into 8 groups of 4:

| Group | Addresses |
|-------|-----------|
| 1 | 0x60–0x63 |
| 2 | 0x64–0x67 |
| 3 | 0x68–0x6B |
| 4 | 0x6C–0x6F |
| 5 | 0x70–0x73 |
| 6 (default) | 0x74–0x77 |
| 7 | 0x78–0x7B |
| 8 | 0x7C–0x7F |

Groups 7 and 8 are outside the 7-bit addresses an I2C master can reach, so avoid them.

## ESPHome

The external component lives at `components/multi_gas/`.

```yaml
esphome:
  name: my-sensor
  libraries:
    - Wire
    - multi-multigas=https://github.com/romkey/multi-multigas#2.0.0

external_components:
  - source: github://romkey/multi-multigas
    components: [multi_gas]

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: arduino

i2c:
  - id: gas_bus
    sda: GPIO21
    scl: GPIO22
  - id: other_bus
    sda: GPIO18
    scl: GPIO19

multi_gas:
  id: gases
  i2c_id: gas_bus
  update_interval: 60s
  debug: true
  detected_gas_sensors:
    name: "Detected Gas Sensors"
  duplicate_gas_sensors:
    name: "Duplicate Gas Sensors"
  co:
    name: "Carbon Monoxide"
  h2s:
    name: "Hydrogen Sulfide"
  o2:
    name: "Oxygen"
```

Notes:

- **`multi_gas:` is correct** — it is a top-level component, not under `sensor:`.
- **`external_components` is required** — use `github://romkey/multi-multigas` (ESPHome finds components in `components/` automatically).
- Use **`i2c_id:`** to select which I2C bus when you have more than one. With a single `i2c:` block, `i2c_id` is optional.
- Use `framework: type: arduino`. The DFRobot dependency needs `Arduino.h` and `Wire.h`, so keep `Wire` in `libraries:` — but note the component itself performs all I2C through the ESPHome bus selected by `i2c_id`, not through `Wire`.
- Set `logger: level: DEBUG` to see the per-address scan, detected gas types, warmup, and skipped publishes. Adding `debug: true` additionally probes the whole 0x08–0x77 range at startup so its output can be compared directly against the `i2c:` component's own `scan: true` results.
- An address that acknowledges on the bus but does not report a supported gas type is logged as a warning (regardless of `debug:`) and retried on every update. Sensors that are still booting behave this way, so they get picked up a minute or two later and the text sensors are republished.
- Only one sensor per gas type can be read, since each gas has a single slot. If two sensors report the same type the lower address wins and the other is logged as a warning rather than silently ignored.
- Configure only the gas sensors you want exposed; each is optional.
- `detected_gas_sensors` is an optional text sensor whose state is the comma-separated list of gas types found on the bus, for example `CO, H2S, O2`. It is published as soon as the scan finishes rather than after the warmup, and lists everything detected even if you did not configure a sensor for it.
- `duplicate_gas_sensors` is an optional text sensor listing every gas type that was found more than once, with the I2C address of each instance, for example `CO (0x60), CO (0x64)`. The first address in each run is the one being read. It is empty when there are no duplicates.
- O2 is reported in **percent**; other gases use **ppm**.
- Sensors need about **3 minutes** to warm up before readings are published.
- If nothing at all answers in 0x60–0x7F at startup, the component marks itself as failed. If something answers but cannot be identified, the component stays up and keeps retrying.
- If you see `multi_gas cannot be loaded via YAML (no CONFIG_SCHEMA)`, ESPHome is importing a leftover component directory that no longer has an `__init__.py`. Delete `.esphome/external_components/` (a "Clean Build Files" does **not** remove it) and restart the ESPHome dashboard or add-on, which also clears its in-memory module cache.

## API

| Method | Description |
|--------|-------------|
| `begin(wire)` | Scan 0x60–0x7F and initialize found sensors. Returns `true` if at least one sensor was found. |
| `has_co()` etc. | Returns whether that gas type was detected. |
| `get_co()` etc. | Read concentration in ppm (or `%` for O2 via DFRobot library). Returns `NaN` if not available. |
| `get_co_raw()` etc. | Read raw sensor voltage. |
| `get_co_i2c_addr()` etc. | I2C address of the detected sensor. |
| `unidentified_count()` / `unidentified_addr(i)` | Addresses that acknowledged but reported no supported gas type. |
| `retry_unidentified()` | Re-run identification on those addresses. Returns how many sensors were newly registered. |
| `duplicate_count()` / `duplicate_addr(i)` / `duplicate_gas(i)` | Sensors whose gas type is already provided by a lower address. |
| `change_addrs(start, end, group, wire)` | Change the address group for sensors in a range. |

## License

MIT
