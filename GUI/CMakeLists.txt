cmake_minimum_required(VERSION 3.16)

# Setup compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the project name
project(GUI)

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

# Search for packages
find_package(Qt6 6.10 REQUIRED COMPONENTS Quick QuickControls2)

# Search for the shared headers; we don't want to hard-code target names we
# determine what gets added when the import is done
get_directory_property(_targets_pre IMPORTED_TARGETS)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../install)
find_package(SharedHeaders QUIET)

if(NOT SharedHeaders_FOUND)
    message(FATAL_ERROR "Shared headers were not found; did you build the firmware project?")
endif()

get_directory_property(_targets_post IMPORTED_TARGETS)

# An intersection of the two sets identifies the new targets
set(SHARED_HEADERS_FILES "")
foreach(_target_name IN LISTS _targets_post)
    if(NOT _target_name IN_LIST _targets_pre)        
        # For each target, obtain header set names
        get_target_property(_all_set_names ${_target_name} INTERFACE_HEADER_SETS)

        foreach(_set_name IN LISTS _all_set_names)
            # For each file set, extract the files and append to list
            get_target_property(_files_in_set ${_target_name} HEADER_SET_${_set_name})
            list(APPEND SHARED_HEADERS_FILES ${_files_in_set})
        endforeach()        
    endif()
endforeach()

# Setup a Qt project
qt_standard_project_setup(REQUIRES 6.10)

# Configure the executable and its source files
qt_add_executable(USBRadio
    "src/main.cpp"
    "src/Device.cpp"
    "src/DeviceManager.cpp"
    "src/DeviceWorker.cpp"
    "src/ReportWorker.cpp"
    ${SHARED_HEADERS_FILES} # We need the Qt's meta-object compiler to process the shared header files since they contain Qt-specific macros
)

# Configure include paths so auto-generated code can find the headers
target_include_directories(USBRadio
    PRIVATE
    "src"
    $<TARGET_PROPERTY:shared-headers,INTERFACE_INCLUDE_DIRECTORIES>
)

# Initialize the QML module target
qt_add_qml_module(USBRadio
    URI USBRadio.Components
    VERSION 1.0
)

# Load the QML files and resources from the subdirectories; this ensures the resource paths do not have subdirectories
add_subdirectory("components")
add_subdirectory("resources")

# Configure hidapi build options
set(HIDAPI_WITH_LIBUSB FALSE)
set(HIDAPI_PRINT_VERSION TRUE)
set(BUILD_SHARED_LIBS FALSE)

# Load the hidapi's own CMake from the subdirectory; this makes 'hidapi::hidapi' target available for linking
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

# Link the binary and the libraries
target_link_libraries(USBRadio
    PRIVATE 
        Qt6::Quick 
        Qt6::QuickControls2 
        hidapi::hidapi
)
