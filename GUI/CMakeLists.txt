cmake_minimum_required(VERSION 3.16)

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

# Search for relevant packages
find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2)

# Setup a Qt project
qt_standard_project_setup(REQUIRES 6.8)

# Configure the executable and its source files
qt_add_executable(USBRadio
    "src/main.cpp"
    "src/tunercontrol.cpp"
)

# Configure the QML portions
qt_add_qml_module(USBRadio
    URI GUI
    VERSION 1.0
    QML_FILES
        "Main.qml"
        "components/TitleBar.qml"
        "components/TunerDial.qml" 
        "components/GlowingText.qml"
        "components/DigitalDisplay.qml"
        "components/RDSPanel.qml"
        "components/ModernButton.qml"
        "components/GradientBar.qml"        
        "components/SignalIndicators.qml"
        "components/StereoIndicator.qml"
        "components/PowerButton.qml"
)

# Configure resources
qt_add_resources(USBRadio
    "resources"
    PREFIX "/"
    FILES 
        "resources/icon.png"
        "resources/backward-solid-full.svg"
        "resources/forward-solid-full.svg"
        "resources/DSEG7ClassicMini-Bold.ttf"
        "resources/DSEG7ClassicMini-Regular.ttf"
        "resources/StereoIndicator.png"
        "resources/PowerButton.png"
)

# Configure hidapi build options
set(HIDAPI_WITH_LIBUSB FALSE)
set(HIDAPI_PRINT_VERSION TRUE)
set(BUILD_SHARED_LIBS FALSE)

# hidapi supports CMake via the "subdirectory" method; this makes 'hidapi::hidapi' target available for linking
add_subdirectory("./ThirdParty/hidapi")

# The debug version attaches a console so console logging works as expected
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(USBRadio
        PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
else()
    set_target_properties(USBRadio
        PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Link the necessary libraries
target_link_libraries(USBRadio
    PRIVATE Qt6::Quick Qt6::QuickControls2 hidapi::hidapi
)
