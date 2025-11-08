cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_HAL_DRIVER 
	STM32F042x6
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc    
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Inc
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/CMSIS/Device/ST/STM32F0xx/Include
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/i2s.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f0xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f0xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/startup/startup_stm32f042c6tx.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/system_stm32f0xx.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usb.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2s.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c_ex.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_exti.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c
    C:/Users/antti/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c
)

# Drivers Middleware

# Link directories setup
set(MX_LINK_DIRS

)

# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}
)

# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

# Add STM32CubeMX generated application sources to the project
target_sources(firmware PRIVATE ${MX_Application_Src})

# Link directories setup
target_link_directories(firmware PRIVATE ${MX_LINK_DIRS})

# Add libraries to the project
target_link_libraries(firmware ${MX_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(firmware PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
