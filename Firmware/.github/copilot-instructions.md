# Firmware Copilot Instructions

## Project Overview
STM32F042x6 embedded firmware for an FM radio receiver (Si4705 chip) with USB audio streaming and HID command interface. Captures tuned FM radio audio via I2S, streams it over USB (audio class), accepts radio commands via HID, and manages radio state via I2C.

When encountering an error or problem, ask for guidance from the user.

## Architecture & Data Flow

1. **Radio Control (Si4705)** → I2C commands via state machine (`Radio/device.h:CommandState_t`, `Radio/commands.c`)
2. **Audio Stream** → I2S DMA half-buffer interrupts feed 48kHz stereo PCM to USB (`Core/main.c:i2sBuffer`, `DMA_BUFFER_LENGTH`)
3. **USB Dual Interface** → TinyUSB (audio class + HID) routes to host
4. **Host Commands** → HID SET_REPORT callbacks parse requests, enqueue radio commands (`USB/hid_callbacks.c`)

## Key Components

| Component | Purpose | Key Files |
|-----------|---------|-----------|
| **Radio State Machine** | Si4705 async I2C command/response via CTS/STC interrupts | `Radio/device.h` (CommandQueue_t, CommandState_t, RadioState_t), `Radio/device.c` |
| **Radio Commands** | Verb-first functions (PowerUp, TuneFreq, SeekStart) that populate Command_t and enqueue | `Radio/commands.c`, `Radio/commands.h` |
| **USB Audio** | PCM streaming via UAC 1.0 at 48kHz stereo | `USB/audio_callbacks.c`, `audio_config.h`, `usb_descriptors.c` |
| **USB HID** | Dual-mode: GET_REPORT (device→host status), SET_REPORT (host→device commands) | `USB/hid_callbacks.c`, `hid_config.h` |
| **I2S + DMA** | Half-buffer interrupt-driven audio capture from Si4705 | `Core/i2s.c`, `Core/dma.c`, `Core/main.c:i2sBuffer` |
| **Main Loop** | HAL init, peripheral setup, radio powerup, command polling | `Core/main.c` |

### Critical Patterns

**Command State Machine** (`Radio/device.h:CommandState_t`): Commands progress through strictly ordered states:
- `IDLE` → `SENDING` (I2C write in main loop) → `WAITING_FOR_CTS` (await Clear-To-Send interrupt)
- → `WAITING_FOR_STC` or `WAITING_FOR_RESPONSE_RETRIEVAL` (depend on command type)
- → `RECEIVING_RESPONSE` (I2C read) → `RESPONSE_RECEIVED` (ready for USB report) → `READY` (done)

This async pattern prevents blocking on I2C transactions. CTS/STC signals are GPIO interrupts that advance state.

**Double-Buffering for Audio** (`Core/main.c`): 
- DMA transfers I2S audio to `i2sBuffer` at 48kHz stereo (192 bytes every ~4ms)
- DMA half-way interrupt (`DMA_BUFFER_MIDPOINT`) signals USB stack to drain first half while DMA fills second half
- Buffer size: `DMA_BUFFER_LENGTH = CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ / 8` (must align with USB config)

**USB Descriptors** (`USB/usb_descriptors.c`): Static audio + HID interface definitions. Audio reports 48kHz stereo PCM via UAC 1.0; HID uses generic IN/OUT. String descriptors are Unicode (uint16_t per character).

## Build & Deployment

The project uses CMake with Ninja + ARM GCC for embedded cross-compilation:
- **CMakeLists.txt**: Defines sources, includes, and HAL/TinyUSB dependencies
- **CMakePresets.json**: Build configuration (toolchain path, optimization flags)
- **cmake/stm32cubemx/**: Auto-generated HAL initialization (DO NOT hand-edit; regenerate via STM32CubeMX)

**Key build variables** (in CMakeLists.txt):
- `CMAKE_C_STANDARD 23` (modern C with designated initializers)
- `add_subdirectory(thirdparty/tinyusb/src)` integrates TinyUSB source

Never build or configure the project manually; workflows are defined in VS Code tasks.

### Flashing
Never flash the firmware. Never run the debugger.

### Key Build Files
- [CMakeLists.txt](CMakeLists.txt): Project structure, sources, includes
- [CMakePresets.json](CMakePresets.json): Build configuration (toolchain, optimization)
- [cmake/stm32cubemx/CMakeLists.txt](cmake/stm32cubemx/CMakeLists.txt): HAL initialization (auto-generated from .ioc)

## Developer Workflows

### Adding a Radio Command
1. **Define opcode** in `Radio/common.h` (CommandIdentifiers_t enum)
2. **Implement command function** in `Radio/commands.c` (e.g., `TuneFreq()`)
   - Populates Command_t structure with args and expected response length
   - Queues via `EnqueueCommand(device, &cmd)` from [Radio/device.c](Radio/device.c#L38)
3. **Add HID report handling** in `USB/hid_callbacks.c` to parse host request and enqueue command

Example: To add a SEEK_START command:
- Define `CMD_ID_FM_SEEK_START` in `Radio/common.h`
- Implement `SeekStart(RadioDevice_t *device, CMD_FM_SEEK_START_ARGS args)` in `Radio/commands.c`
- Parse the seek request in [USB/hid_callbacks.c:tud_hid_set_report_cb()](USB/hid_callbacks.c#L56) and call `SeekStart()`

### Adjusting Audio Streaming
- **Sampling rate**: Changed in [Core/main.c:REAL_AUDIO_FREQUENCY](Core/main.c#L35) and `audio_config.h:CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE`
- **Buffer size**: Adjust `DMA_BUFFER_LENGTH` in [main.c](Core/main.c#L30) (must match USB config `CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ`)
- **Channel count**: Edit [usb_descriptors.c](USB/usb_descriptors.c) audio descriptor (currently stereo)

### Debugging I2C/Radio Issues
- Use **I2C trace** via [Core/i2c.c](Core/i2c.c) callbacks (HAL provides `HAL_I2C_MasterRxCpltCallback`, etc.)
- Check **CTS/STC GPIO pins** (interrupt sources in [Radio/device.h](Radio/device.h#L40-L55))
- Monitor **command queue state** in debugger via `RadioDevice_t.commandQueue` structure

## Project-Specific Conventions

- **Source style**: Doxygen comments for functions; underscore-prefixed private types (e.g., `_Command_t`)
- **Naming**: Radio functions are imperative + verb-first (PowerUp, SeekStart, TuneFreq)
- **Memory**: Stack-allocated command queue (10 entries max) in device struct; no dynamic allocation
- **Interrupt priorities**: USB highest (0), others lower to avoid interference

## Integration Points

- **STM32CubeMX** (`Generated/Firmware.ioc`): Regenerate HAL with MX tool; do NOT hand-edit generated files in `cmake/stm32cubemx/`
- **TinyUSB** (`thirdparty/tinyusb/src`): Audio/HID class drivers; configs in `USB/*_config.h`
- **Host Communication**: Binary HID reports defined by USB descriptor; GUI sends/receives via hidapi
