from pathlib import Path

import pytest
from esphome import config as esphome_config
from esphome.const import CONF_UNIT_OF_MEASUREMENT, UNIT_PARTS_PER_MILLION, UNIT_PERCENT

TESTS = Path(__file__).resolve().parent


def test_valid_config_loads():
    esphome_config.load_config(str(TESTS / "test.yaml"))


def test_missing_i2c_raises():
    with pytest.raises(Exception):
        esphome_config.load_config(str(TESTS / "test_invalid_no_i2c.yaml"))


def test_o2_sensor_uses_percent(multi_gas_module):
    config = multi_gas_module.O2_SENSOR_SCHEMA({"name": "Oxygen"})
    assert config[CONF_UNIT_OF_MEASUREMENT] == UNIT_PERCENT


def test_ppm_sensor_uses_ppm(multi_gas_module):
    config = multi_gas_module.PPM_SENSOR_SCHEMA({"name": "Carbon Monoxide"})
    assert config[CONF_UNIT_OF_MEASUREMENT] == UNIT_PARTS_PER_MILLION


def test_all_gas_sensors_are_optional(multi_gas_module):
    config = multi_gas_module.CONFIG_SCHEMA({"id": "gas_sensors"})
    assert config["id"] is not None


def test_unknown_gas_key_rejected(multi_gas_module):
    with pytest.raises(Exception):
        multi_gas_module.CONFIG_SCHEMA(
            {
                "id": "gas_sensors",
                "not_a_gas": {"name": "Nope"},
            }
        )
