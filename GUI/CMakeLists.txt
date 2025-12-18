cmake_minimum_required(VERSION 4.0)

# Setup compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set the project name
project(usb-fm-radio)

# Enable CMake support for C++ language
enable_language(CXX)

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

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appGUI
    main.cpp
)

qt_add_qml_module(appGUI
    URI GUI
    VERSION 1.0
    QML_FILES
        Main.qml
)

set_target_properties(appGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appGUI
    PRIVATE Qt6::Quick
)
