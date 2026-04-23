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

# Create an executable object type
add_executable(firmware)

# Add the shared header library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add STM32CubeMX generated sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32cubemx)

# Add TinyUSB functions
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyusb/src)

# Add librdsparser to the project
set(RDSPARSER_DISABLE_HEAP ON)
set(RDSPARSER_DISABLE_TESTS ON)
set(RDSPARSER_DISABLE_EXAMPLES ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/librdsparser)

# Linker search path setup
target_link_directories(firmware PRIVATE
    # Add user defined library search paths
)

# Add include paths
target_include_directories(firmware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/USB
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/librdsparser/include
)

# Add sources to executable
target_sources(firmware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/commands.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/properties.c

    # librdsparser requires callbacks defined by the user
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/rds.c
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

# Add linked libraries
target_link_libraries(firmware
    shared-headers
    stm32cubemx
    rdsparser_static
)
