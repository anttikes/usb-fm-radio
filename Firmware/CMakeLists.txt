cmake_minimum_required(VERSION 4.0)

# Setup compiler settings
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Set the project name
project(Firmware)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# If you need to debug the environment or CMake variables then uncomment the following lines
#message("Environment variables: ")
#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

#message("CMAKE variables: ")
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Add the shared header library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add TinyUSB functions, and get the sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyusb/src)
tinyusb_sources_get(TINYUSB_SOURCES)

# Build TinyUSB as a static library
add_library(tinyusb STATIC ${TINYUSB_SOURCES})

target_include_directories(tinyusb PRIVATE
    # Ensure TinyUSB can find our tusb_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/USB

    # Ensure TinyUSB can find its own tusb_option.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/    
)

# Optimize TinyUSB code so it can fit into the flash memory
target_compile_options(tinyusb PRIVATE -Os -g0)

target_link_libraries(tinyusb
    # Ensure TinyUSB is aware of the shared headers ("reports.h" contains the HID report size definition)
    shared-headers
)

# Add librdsparser to the project
set(RDSPARSER_DISABLE_HEAP ON CACHE BOOL "Disable heap allocator (rdsparser_new/free)")
set(RDSPARSER_DISABLE_TESTS ON CACHE BOOL "Disable tests")
set(RDSPARSER_DISABLE_EXAMPLES ON CACHE BOOL "Disable examples")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/librdsparser)

# Create an executable object for the firmware
add_executable(firmware)

# Add STM32CubeMX generated sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32cubemx)

# Add include paths
target_include_directories(firmware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/USB
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/librdsparser/include
)

# Add sources to executable
target_sources(firmware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/commands.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/properties.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/rds.c

    # TODO: Should the librdsparser callbacks be in a separate file?

    # TinyUSB requires callbacks and descriptors defined by the user
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/audio_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/hid_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/usb_descriptors.c    
    
    # Use the FSDEV driver for STM32 from TinyUSB
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c    
)

# Add linked libraries
target_link_libraries(firmware
    shared-headers
    tinyusb    
    stm32cubemx
    rdsparser_static
)
