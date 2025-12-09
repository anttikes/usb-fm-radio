cmake_minimum_required(VERSION 4.0)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Set the project name
set(PROJECT_NAME "firmware")
project("${PROJECT_NAME}")

# If you need to debug the environment variables, uncomment these two lines
# message("Environment variables: ")
# execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

# If you need to debug the CMake variables, uncomment these lines
# message("CMAKE variables: ")
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable("${PROJECT_NAME}")

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add TinyUSB functions
add_subdirectory(thirdparty/tinyusb/src)

# Linker search path setup
target_link_directories("${PROJECT_NAME}" PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources("${PROJECT_NAME}" PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/Src/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/Src/commands.c
)

# Add include paths
target_include_directories("${PROJECT_NAME}" PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/
)

# Add TinyUSB sources to the project
tinyusb_target_add("${PROJECT_NAME}")
target_sources("${PROJECT_NAME}" PRIVATE
    # TinyUSB requires callbacks and descriptors defined by the user
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Src/audio_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Src/hid_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Src/usb_descriptors.c    
    
    # Use the provided FSDEV driver
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c    
)

# Add project symbols (macros)
target_compile_definitions("${PROJECT_NAME}" PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries("${PROJECT_NAME}"
    stm32cubemx

    # Add user defined libraries
)
