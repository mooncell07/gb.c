from ctypes import cdll, c_bool, c_uint16, c_uint8, Structure, c_int, c_size_t
import json
import os

LIB_PATH = "./build/test/libreGB.so"
TEST_PATH = "./sm83/v1/"


class RAM(Structure):
    _fields_ = [("address", c_uint16), ("data", c_uint8)]


class Config(Structure):
    _fields_ = [
        ("initial", c_bool),
        ("PC", c_uint16),
        ("SP", c_uint16),
        ("A", c_uint8),
        ("F", c_uint8),
        ("B", c_uint8),
        ("C", c_uint8),
        ("D", c_uint8),
        ("E", c_uint8),
        ("H", c_uint8),
        ("L", c_uint8),
        ("ram", RAM * 0xF),
        ("ramSize", c_size_t),
    ]


class Cycles(Structure):
    _fields_ = [("address", c_uint16), ("data", c_uint8)]


class ConfigCycles(Structure):
    _fields_ = [("cycles", Cycles * 0xF), ("count", c_size_t)]


def build_config(test: dict, initial: bool) -> Config:
    config = Config()
    config.initial = initial
    config.A = test["a"]
    config.F = test["f"]
    config.B = test["b"]
    config.C = test["c"]
    config.D = test["d"]
    config.E = test["e"]
    config.H = test["h"]
    config.L = test["l"]
    config.PC = test["pc"]
    config.SP = test["sp"]

    ram_size = 0
    for i, entry in enumerate(test["ram"]):
        config.ram[i] = RAM(address=entry[0], data=entry[1])
        ram_size += 1
    config.ramSize = ram_size
    return config


def build_config_cycles(test_cycles: dict) -> ConfigCycles:
    cycles = (Cycles * 0xF)()
    cycles_count = 0
    for i, entry in enumerate(test_cycles):
        cycles[i] = Cycles(address=entry[0], data=entry[1])
        cycles_count += 1

    config = ConfigCycles()
    config.cycles = cycles
    config.count = cycles_count
    return config


def main() -> None:
    regb = cdll.LoadLibrary(LIB_PATH)
    regb.runTest.argtypes = [Config, Config, ConfigCycles]
    regb.runTest.restype = c_int

    total_tests = 0
    passed_tests = 0

    passed_cases = 0

    for filename in os.listdir(TEST_PATH):
        total_tests += 1
        if filename in ("10.json", "76.json"):
            # Skipping STOP and HALT
            print(f"RUNNER: SKIPPING `{filename}`")
            continue
        with open(TEST_PATH + filename) as file:
            tests = json.loads(file.read())
            for test in tests:
                config_initial = build_config(test["initial"], initial=True)
                config_final = build_config(test["final"], initial=False)
                config_cycles = build_config_cycles(test["cycles"])

                status = regb.runTest(config_initial, config_final, config_cycles)
                if status == 1:
                    print(f"\nRUNNER: FAILED `{test["name"]}`!")
                    exit(1)
                passed_cases += 1

            print(f"RUNNER: PASSED `{test["name"]}`!")
            passed_tests += 1

    print("\nFinished running the sm83 JSON Tests.")
    print(
        f"Total Tests: {total_tests} | Passed Tests: {passed_tests} | Skipped Tests: {total_tests - passed_tests}"
    )
    print(f"Total Passed Test Cases: {passed_cases}")


main()
