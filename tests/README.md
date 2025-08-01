# Tests

We run the JSON Tests by loading the shared object in `runner.py` script and passing it the initial, final and cycles
config from the sm83 JSON Tests.

# Usage Instructions

Run the following commands:

```
cmake --preset test
cmake --build --preset test
python tests/runner.py
```

and finally, to address the elephant in the room, i just cba doing JSON parsing in C, so.. yeah.
