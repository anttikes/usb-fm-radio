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
project(usb-fm-radio)

# If you need to debug the environment or CMake variables then uncomment the following lines
message("Environment variables: ")
execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

message("CMAKE variables: ")
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME usb-fm-radio)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if("${BUILD_TARGET}" STREQUAL "firmware")
    add_subdirectory(Firmware)
elseif("${BUILD_TARGET}" STREQUAL "gui")
    add_subdirectory(GUI)
endif()
