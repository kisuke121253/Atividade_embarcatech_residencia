# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(atividade C CXX ASM)

# Inicializa o SDK da Pico
pico_sdk_init()

# Define os arquivos fonte organizados por pastas
add_executable(atividade
    app/main.c
    drivers/led_embutido.c
    hal/hal_led.c
)

# Nome e versão do programa
pico_set_program_name(atividade "atividade")
pico_set_program_version(atividade "0.1")

# Saída USB ativada, UART desativado (ajuste conforme necessário)
pico_enable_stdio_uart(atividade 0)
pico_enable_stdio_usb(atividade 1)

# Diretórios com headers
target_include_directories(atividade PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Bibliotecas utilizadas (incluindo suporte ao LED do Wi-Fi)
target_link_libraries(atividade
    pico_stdlib
    pico_cyw43_arch_none
)

# Geração de arquivos binários/UF2
pico_add_extra_outputs(atividade)
