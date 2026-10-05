# Mini DJ Controller — embedded lighting prototype

A hands-on microcontroller project exploring two-channel PWM lighting, serial controls, enclosure design, and prototype electronics. The current firmware targets the **Raspberry Pi Pico (RP2040)**; earlier ESP32 BLE iterations are preserved under `legacy/`.

## Implemented scope

- Two PWM output channels on GPIO 5 and 18.
- Manual brightness, breathing, crossfade, and strobe modes.
- Serial commands for channel values and effect selection; the V2 sketch adds an effect-speed command.
- STEP/STL enclosure designs, schematic/PCB images, a component BOM, photographs, and a build log.

This is a **hardware prototype**, not a finished audio DJ mixer. Microphone input, audio recording/remixing, AI visuals, display integration, and final PCB/assembly validation remain future work. Images describing a final concept are design intentions, not proof of a manufactured product.

## Repository structure

| Directory | Purpose |
| --- | --- |
| `Firmware/` | Current Pico sketches |
| `CAD/` | Enclosure STEP/STL files and design images |
| `PCB/` | Early schematics and board-layout images |
| `Docs/` | Bill of materials and build log |
| `Images/` | Prototype photos and development screenshots |
| `legacy/esp32-ble-prototype/` | Earlier BLE firmware/browser controller, case ZIP, and records |
| `legacy/cyber-dj/` | Original BLE controller and original demonstration media |

## Firmware usage

Open one sketch from `Firmware/` in an Arduino-compatible RP2040 toolchain and select the matching Pico board. Inspect the pin assignments and schematic before connecting a load; do not assume a GPIO can drive a lamp directly.

The serial interface runs at 115200 baud:

```text
A 128    # channel A brightness, 0–255
B 64     # channel B brightness, 0–255
E 1      # effect: 0 manual, 1 breathe, 2 crossfade, 3 strobe
S 30     # V2 only: effect speed, 1–100
```

Firmware compilation and physical output behavior have not been re-tested during portfolio curation. Hardware reproducibility and assembly testing are still needed.

## Engineering focus

The project records the transition from browser/BLE-controlled ESP32 firmware to a serial-controlled Pico design, with separate electrical, mechanical, and software artifacts. See [consolidation provenance](docs/PROVENANCE.md) for preserved source repositories and history.

## Licensing

Existing license files remain in their original imported directories. In particular, `legacy/cyber-dj/LICENSE` applies to that imported material. No new blanket license is asserted for the complete hardware collection.
