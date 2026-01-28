# Firmware Copilot Instructions

## Project Overview
This is an embedded firmware for STM32F042x6 microcontroller controlling an FM radio receiver (Si4705 chip) with USB audio streaming and HID command interface.

When encountering an error or problem, ask for guidance from the user.

**Core purposes:** 
- Capture tuned FM radio audio via I2S and stream it over USB (audio class)
- Accept radio commands via HID and control the Si4705 over I2C
- Manage radio state machine and command queue, and report status via HID reports

## Architecture

### High-Level Data Flow
1. **Radio (Si4705)** → I2C commands (`Radio/commands.c`)
2. **Audio out** → I2S DMA buffer (`Core/i2s.c`, `main.c:i2sBuffer`)
3. **USB Interface** → TinyUSB stack handles both audio & HID
4. **Host Commands** → HID callbacks parse requests and queue radio commands (`USB/hid_callbacks.c`)

### Key Components

| Component | Purpose | Key Files |
|-----------|---------|-----------|
| **Radio Control** | Si4705 command/response state machine | `Radio/device.h` (CommandQueue_t, RadioState_t) |
| **USB Audio** | PCM streaming via UAC 1.0 | `USB/audio_callbacks.c`, `audio_config.h` |
| **USB HID** | Control commands from host | `USB/hid_callbacks.c`, `hid_config.h` |
| **Peripherals** | GPIO, I2C, I2S, DMA, Timer | `Core/*.c` (generated + manual code) |
| **Main Loop** | Initializes, polls command queue | `Core/main.c` |

### Critical Patterns

**Command State Machine** (`Radio/device.h`): Commands move through states: IDLE → SENDING → WAITING_FOR_CTS/STC → RESPONSE_RECEIVED → READY. This handles async I2C transactions with interrupt-driven CTS/STC signals from the radio.

**Double-Buffering for Audio** (`main.c`): DMA_BUFFER uses half-way interrupt to fill while USB drains (prevents tearing). Frequency: 48kHz.

**USB Descriptors**: Defined statically in `usb_descriptors.c`. Audio interface reports 48kHz stereo PCM; HID reports control codes for radio commands.

## Build & Deployment
The project is configured by `CMakeLists.txt`. Uses:
- **Compiler**: ARM GCC (gcc-arm-none-eabi)
- **Build System**: Ninja
- **Dependencies**: STM32CubeF0 HAL, TinyUSB, STM32CubeMX-generated files

Never build or configure the project.

### Flashing
Never flash the firmware. Never run the debugger.

### Key Build Files
- `CMakeLists.txt`: Project structure, sources, includes
- `CMakePresets.json`: Build configuration (toolchain, optimization)
- `cmake/stm32cubemx/`: HAL initialization (auto-generated)

## Developer Workflows

### Adding a Radio Command
1. **Define opcode** in `Radio/common.h` (CommandIdentifiers_t enum)
2. **Implement command function** in `Radio/commands.c` (e.g., `TuneFreq()`)
   - Populates Command_t structure with args and expected response length
   - Queues via device's command queue
3. **Add HID report handling** in `USB/hid_callbacks.c` to parse host request and enqueue command

### Adjusting Audio Streaming
- **Sampling rate**: Changed in `Core/main.c:REAL_AUDIO_FREQUENCY` and `audio_config.h:CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE`
- **Buffer size**: Adjust `DMA_BUFFER_LENGTH` in `main.c` (must match USB config)
- **Channel count**: Edit `usb_descriptors.c` audio descriptor (currently stereo)

### Debugging I2C/Radio Issues
- Use **I2C trace** via `Core/i2c.c` callbacks (HAL)
- Check **CTS/STC GPIO pins** (interrupt sources in `Radio/device.h:CommandState_t`)
- Monitor **command queue state** in debugger (`Radio/device.h:CommandQueue_t`)

## Project-Specific Conventions

- **Source style**: Doxygen comments for functions; underscore-prefixed private types (e.g., `_Command_t`)
- **Naming**: Radio functions are imperative + verb-first (PowerUp, SeekStart, TuneFreq)
- **Memory**: Stack-allocated command queue (10 entries max) in device struct; no dynamic allocation
- **Interrupt priorities**: USB highest (0), others lower to avoid interference

## Integration Points

- **STM32CubeMX** (`Generated/Firmware.ioc`): Regenerate HAL with MX tool; do NOT hand-edit generated files in `cmake/stm32cubemx/`
- **TinyUSB** (`thirdparty/tinyusb/src`): Audio/HID class drivers; configs in `USB/*_config.h`
- **Host Communication**: Binary HID reports defined by USB descriptor; GUI sends/receives via hidapi

## Common Gotchas

1. **I2S frequency mismatch**: REAL_AUDIO_FREQUENCY must match system clock config or audio quality degrades
2. **DMA buffer alignment**: Must be 2-byte aligned for I2S transfers
3. **USB string descriptors**: Must use Unicode format (uint16_t per character) in `usb_descriptors.c`
4. **Radio CTS timeout**: If command never completes, check that interrupt GPIO is configured correctly
