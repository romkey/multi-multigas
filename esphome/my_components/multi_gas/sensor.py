import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_EMPTY, UNIT_PARTS_PER_MILLION, STATE_CLASS_MEASUREMENT

multi_gas_ns = cg.esphome_ns.namespace('multi_gas')

MultiGas = multi_gas_ns.class_('MultiGas', cg.PollingComponent)

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

SENSORS = [
    CONF_CL2,
    CONF_CO,
    CONF_H2,
    CONF_H2S,
    CONF_HF,
    CONF_HCL,
    CONF_O2,
    CONF_O3,
    CONF_NH3,
    CONF_NO2,
    CONF_PH3,
    CONF_SO2,
]

#CONFIG_SCHEMA = sensor.sensor_schema(UNIT_EMPTY, ICON_EMPTY, 1).extend({
#    cv.GenerateID(): cv.declare_id(EmptySensor)
#}).extend(cv.polling_component_schema('60s'))
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MultiGas),
            cv.Optional(CONF_CL2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_CO): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_H2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_H2S): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_HF): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_HCL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_O2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_O3): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_NH3): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_NO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_PH3): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_SO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                state_class=STATE_CLASS_MEASUREMENT,
                accuracy_decimals=2,
            ),
            })
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await  cg.register_component(var, config)

    for sensor_type in SENSORS:
        if sensor_type in config:
            sens = await sensor.new_sensor(config[sensor_type])
            cg.add(getattr(var, f"set_{sensor_type}_sensor")(sens))
            
#    yield sensor.register_sensor(var, config)
