cmake_minimum_required(VERSION 4.0)

# Create an interface library to hold the headers shared with the GUI project
add_library(shared-headers INTERFACE)

# Define a file set for the headers to include in the installation
target_sources(shared-headers INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/constants.h
        ${CMAKE_CURRENT_SOURCE_DIR}/reports.h
)

# The CMakePresets.json plays an important role here;
# it specifies the base directory under which all these get put

# Create an install rule which specifies where the files are put
install(
    TARGETS shared-headers 
    EXPORT shared-headers
    INCLUDES DESTINATION include
    FILE_SET HEADERS
)

# Specify the file name and relative path for the export target
install(EXPORT shared-headers
    FILE SharedHeadersTarget.cmake
    DESTINATION lib/cmake/SharedHeaders
)

include(CMakePackageConfigHelpers)

# Generate the configuration file from the template, 
# and specify where it gets installed
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

# Create a custom target included in the meta-target 'ALL' that
# performs the install step as part of the overall build process
add_custom_target(install_headers ALL
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    COMMENT "Installing common header files..."
)
