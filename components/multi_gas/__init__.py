import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor, text_sensor
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    UNIT_PARTS_PER_MILLION,
    UNIT_PERCENT,
)

CODEOWNERS = ["@romkey"]
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "text_sensor"]

multi_gas_ns = cg.esphome_ns.namespace("multi_gas")
MultiGas = multi_gas_ns.class_("MultiGas", cg.PollingComponent, i2c.I2CDevice)

CONF_DEBUG = "debug"
CONF_DETECTED_GAS_SENSORS = "detected_gas_sensors"
CONF_DUPLICATE_GAS_SENSORS = "duplicate_gas_sensors"

CONF_CL2 = "cl2"
CONF_CO = "co"
CONF_H2 = "h2"
CONF_H2S = "h2s"
CONF_HF = "hf"
CONF_HCL = "hcl"
CONF_O2 = "o2"
CONF_O3 = "o3"
CONF_NH3 = "nh3"
CONF_NO2 = "no2"
CONF_PH3 = "ph3"
CONF_SO2 = "so2"

PPM_SENSORS = [
    CONF_CL2,
    CONF_CO,
    CONF_H2,
    CONF_H2S,
    CONF_HF,
    CONF_HCL,
    CONF_O3,
    CONF_NH3,
    CONF_NO2,
    CONF_PH3,
    CONF_SO2,
]

SENSORS = PPM_SENSORS + [CONF_O2]

PPM_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_PARTS_PER_MILLION,
    state_class=STATE_CLASS_MEASUREMENT,
    accuracy_decimals=2,
)

O2_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_PERCENT,
    state_class=STATE_CLASS_MEASUREMENT,
    accuracy_decimals=2,
)

TEXT_SENSORS = [CONF_DETECTED_GAS_SENSORS, CONF_DUPLICATE_GAS_SENSORS]

TEXT_SENSOR_SCHEMA = text_sensor.text_sensor_schema()

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MultiGas),
            cv.Optional(CONF_DEBUG, default=False): cv.boolean,
            cv.Optional(CONF_O2): O2_SENSOR_SCHEMA,
        }
    )
    .extend({cv.Optional(name): TEXT_SENSOR_SCHEMA for name in TEXT_SENSORS})
    .extend({cv.Optional(name): PPM_SENSOR_SCHEMA for name in PPM_SENSORS})
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x60))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_debug(config[CONF_DEBUG]))

    for sensor_type in SENSORS:
        if sensor_type in config:
            sens = await sensor.new_sensor(config[sensor_type])
            cg.add(getattr(var, f"set_{sensor_type}_sensor")(sens))

    for text_sensor_type in TEXT_SENSORS:
        if text_sensor_type in config:
            text_sens = await text_sensor.new_text_sensor(config[text_sensor_type])
            cg.add(getattr(var, f"set_{text_sensor_type}_text_sensor")(text_sens))
