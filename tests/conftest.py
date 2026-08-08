import importlib.util
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parent.parent
TESTS = Path(__file__).resolve().parent


@pytest.fixture(scope="session")
def multi_gas_module():
    init_path = ROOT / "esphome" / "components" / "multi_gas" / "__init__.py"
    spec = importlib.util.spec_from_file_location("multi_gas", init_path)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    return module
