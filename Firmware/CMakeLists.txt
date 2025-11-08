# Create an executable object type
add_executable(firmware)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add TinyUSB functions
add_subdirectory(thirdparty/tinyusb/src)

# Link directories setup
target_link_directories(firmware PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(firmware PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/si4705.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Src/audio_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Src/hid_callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Src/usb_descriptors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
)

# Add TinyUSB sources to the project
add_library(tinyusb OBJECT)
tinyusb_target_add(tinyusb)
target_include_directories(tinyusb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/
)

# Add include paths
target_include_directories(firmware PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/USB/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyusb/src/portable/st/stm32_fsdev/
)

# Add project symbols (macros)
target_compile_definitions(firmware PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(firmware
    stm32cubemx

    # Add user defined libraries
    tinyusb
)