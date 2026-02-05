cmake_minimum_required(VERSION 4.0)

# Setup compiler settings
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Set the project name
project(usb-fm-radio)

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

# Create an executable object type
add_executable(firmware)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add TinyUSB functions
add_subdirectory(thirdparty/tinyusb/src)

# Linker search path setup
target_link_directories(firmware PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(firmware PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/commands.c
)

# Add include paths
target_include_directories(firmware PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/USB
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/
)

# Add TinyUSB sources to the project
tinyusb_target_add(firmware)
target_sources(firmware PRIVATE
    # TinyUSB requires callbacks and descriptors defined by the user
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/audio_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/hid_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/usb_descriptors.c    
    
    # Use the FSDEV driver for STM32
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c    
)

# Add project symbols (macros)
target_compile_definitions(firmware PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(firmware
    stm32cubemx

    # Add user defined libraries
)