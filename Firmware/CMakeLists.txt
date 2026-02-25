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

# Create an interface library to hold the headers shared with the GUI project
add_library(shared-headers INTERFACE)

# Define a file set for the headers to include
target_sources(shared-headers INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Include
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/constants.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/reports.h
)

# Create an install rule for the shared headers, including an export target
install(
    TARGETS shared-headers 
    EXPORT shared-headers-target
    INCLUDES DESTINATION include
    FILE_SET HEADERS
)

# Specify the file name and relative path for the export target
install(EXPORT shared-headers-target
    FILE SharedHeadersTarget.cmake
    DESTINATION lib/cmake/SharedHeaders
)

include(CMakePackageConfigHelpers)

# Generate the configuration file from the template
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharedHeadersConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharedHeadersConfig.cmake"
    INSTALL_DESTINATION lib/cmake/SharedHeaders
)

# Install the generated configuration file
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/SharedHeadersConfig.cmake"
    DESTINATION lib/cmake/SharedHeaders
)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/commands.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Radio/properties.c
)

# Add include paths
target_include_directories(firmware PRIVATE
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

# Add linked libraries
target_link_libraries(firmware
    shared-headers
    stm32cubemx    
)

# Create a custom target included in the meta-target 'ALL' that
# performs the install step as part of the overall build process
add_custom_target(install_headers ALL
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    COMMENT "Installing common header files..."
)