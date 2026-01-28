# GUI Copilot Instructions

## Project Overview
Qt 6.10.1 desktop application for controlling the FM radio device over USB HID. Displays tuner dial, RDS info, signal strength, and sends radio commands (tune, seek, power) to the firmware.

When encountering an error or problem, ask for guidance from the user.

**Core purpose:** Provide interactive radio control UI that communicates with firmware via HID reports over USB.

## Architecture

### High-Level Data Flow
1. **USB Device Detection** → HID enumeration (`DeviceManager`)
2. **User UI Actions** → QML signals to C++ slots
3. **Command Serialization** → Binary HID reports sent to device
4. **Report Polling** → Firmware responses (RSQ status) read in background thread
5. **UI Update** → Signals update QML model properties

### Key Components

| Component | Purpose | Key Files |
|-----------|---------|-----------|
| **Device Discovery** | Detect USB radio devices, manage selection | `src/DeviceManager.{h,cpp}` (QML singleton) |
| **Device Communication** | Open/close HID device handles | `src/Device.{h,cpp}` (Q_GADGET for QML) |
| **Background Workers** | Poll device status, send commands without blocking UI | `src/DeviceWorker.cpp`, `src/ReportWorker.cpp` |
| **Report Parsing** | Unmarshal binary HID responses to structs | `src/Reports.h` (RSQStatusReport, etc.) |
| **QML UI** | Radio interface with tuner dial, buttons, displays | `Main.qml`, `components/*.qml` |

### Critical Patterns

**QML-C++ Integration**: Use Q_PROPERTY + signals/slots for reactive updates. Device list is exposed via `DeviceManager::devices()` as Q_PROPERTY, automatically refreshes QML when `devicesChanged()` emitted.

**Worker Threads**: `DeviceWorker` and `ReportWorker` run in background to prevent UI freeze:
- `DeviceWorker`: Detects USB plug/unplug events
- `ReportWorker`: Polls HID input reports (~100ms timer) for RSQ status updates
- Both emit signals to main thread, which updates properties

**HID Protocol**: 
- **Commands**: QML calls C++ method → serializes to HID report → sent via hidapi
- **Responses**: Binary reports read via `hid_read()` → parsed into `RSQStatusReport` → emitted as signal → QML updates display

**QML Components**: Custom components use properties + animations for visual effects (e.g., TunerDial rotation, GlowingText color pulsing).

## Build & Debug
The project is configured by:
- `CMakeLists.txt`: Qt module setup, MOC registration, QML files
- Qt standard setup: `qt_standard_project_setup()`, `qt_add_qml_module()`

Never build or configure the project.

### Debugging
Never debug the project.

### Key Runtime Dependencies
- Qt 6.10.1 libraries (DLLs)
- MinGW 13.1.0 runtime
- HIDAPI library (Windows HID subsystem wrapper)

## Developer Workflows

### Adding a New Radio Control
1. **Define HID report structure** in `Reports.h` (if not exists)
2. **Add C++ command method** in `DeviceManager` or `Device` class
   - Serializes command to HID report using HIDAPI `hid_write()`
3. **Expose to QML** via `Q_INVOKABLE` method or Q_PROPERTY
4. **Connect UI button/slider** in QML to call method
5. **Test**: Build, detect device, trigger control, watch firmware response

Example (tune to frequency):
```cpp
// Device.cpp
void Device::tuneFrequency(uint16_t frequency) {
    uint8_t report[64] = {/* HID report format */};
    hid_write(m_deviceHandle, report, sizeof(report));
}
```

### Updating UI Layout
1. **Add QML component** to `Main.qml` or new file in `components/`
2. **Bind properties** to DeviceManager data (e.g., `DeviceManager.selectedDevice.frequency`)
3. **Connect signals** for user interaction (e.g., `onClicked`, `onDrag`)
4. **Register fonts/resources** in `CMakeLists.txt` if needed (DSEG7 fonts in `resources/`)

### Handling New Device Reports
1. **Parse HID report structure** from firmware `usb_descriptors.c` → define in `Reports.h`
2. **Add signal** to `ReportWorker` emitting parsed report (e.g., `rsqStatusReportReceived()`)
3. **Connect signal** in `DeviceManager` to update state
4. **Bind QML property** to display (e.g., `Text { text: Manager.signalStrength }`)

## Project-Specific Conventions

- **Naming**: Classes use CamelCase; HID structures prefixed with command/report type (e.g., `RSQStatusReport`)
- **Threading**: All HID I/O in worker threads; main thread only updates UI
- **Memory**: Use Qt containers (QList, QString) not std:: (better Qt integration)
- **QML Style**: Use "Basic" style (cross-platform, lightweight)
- **Signals/Slots**: Prefer `connect()` calls to lambda functions for complex handlers

## Integration Points

- **Firmware Communication**: Binary HID reports; format defined in firmware `usb_descriptors.c` and parsed in `Reports.h`
- **USB Device Enumeration**: HIDAPI (`hid_enumerate()`) on Windows via SetupAPI
- **Qt Framework**: All UI composition via QML + C++ backend (Qt Creator MOC system)
- **Custom Fonts**: DSEG7 for digital display (loaded via QRC resources)

## Common Gotchas

1. **Thread safety**: Never call HID functions from QML thread; must queue to worker via `QThreadPool` or direct thread
2. **Report format mismatch**: HID report ID and structure must match firmware descriptor exactly, or commands silently fail
3. **Unicode strings**: Qt uses UTF-16 internally; device info strings from HIDAPI are UTF-8 on Windows, convert with `QString::fromUtf8()`
4. **Timer granularity**: `ReportWorker::m_timer` at ~100ms; faster polling increases USB load
5. **Device disconnection**: `hid_read()` returns -1 on disconnect; worker must detect and emit `devicesChanged()` signal
