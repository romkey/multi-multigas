import os
import subprocess
from pathlib import Path

import pytest
from esphome.const import CONF_UNIT_OF_MEASUREMENT, UNIT_PARTS_PER_MILLION, UNIT_PERCENT

ROOT = Path(__file__).resolve().parent.parent
TESTS = Path(__file__).resolve().parent
TEST_CONFIG = Path(os.environ.get("ESPHOME_TEST_CONFIG", TESTS / "test.yaml"))


def test_valid_config_loads():
    subprocess.run(
        ["esphome", "config", str(TEST_CONFIG)],
        check=True,
        cwd=ROOT,
    )


def test_missing_i2c_raises():
    result = subprocess.run(
        ["esphome", "config", str(TESTS / "test_invalid_no_i2c.yaml")],
        cwd=ROOT,
        capture_output=True,
        text=True,
    )
    assert result.returncode != 0


def test_o2_sensor_uses_percent(multi_gas_module):
    config = multi_gas_module.O2_SENSOR_SCHEMA({"name": "Oxygen"})
    assert config[CONF_UNIT_OF_MEASUREMENT] == UNIT_PERCENT


def test_ppm_sensor_uses_ppm(multi_gas_module):
    config = multi_gas_module.PPM_SENSOR_SCHEMA({"name": "Carbon Monoxide"})
    assert config[CONF_UNIT_OF_MEASUREMENT] == UNIT_PARTS_PER_MILLION


def test_all_gas_sensors_are_optional(multi_gas_module):
    config = multi_gas_module.CONFIG_SCHEMA(
        {
            "id": "gas_sensors",
            "i2c_id": "bus",
        }
    )
    assert config["id"] is not None


def test_unknown_gas_key_rejected(multi_gas_module):
    with pytest.raises(Exception):
        multi_gas_module.CONFIG_SCHEMA(
            {
                "id": "gas_sensors",
                "i2c_id": "bus",
                "not_a_gas": {"name": "Nope"},
            }
        )
