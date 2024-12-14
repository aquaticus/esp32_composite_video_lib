# ESP32 Composite Video Pattern Example

This example showcases the Philips PM5544 test pattern, which can be used to verify hardware and connection integrity.

## Build

```bash
cd examples/test_pattern
idf.py build
idf.py flash
```

## Video mode

By default, the example generates a PAL/SECAM signal.

To generate NTSC video, use `menuconfig` to change the mode.

1. Execute `idf.py menuconfig`.
2. Go to `Component config->Example Configuration` 
3. Check `NTSC video mode instead of default PAL/SECAM`.

## Video output

![Philips PM5544 test pattern](../../doc/pattern.jpg)
