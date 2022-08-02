cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME Twin) # <-- Name your project/executable here!

set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add your source files
#add_executable(${NAME}
#    jdBlink1.cpp # <-- Add source files here!
#)

# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project
#include(../pimoroni-pico/libraries/pico_explorer/pico_explorer.cmake)
# include(../pimoroni-pico/libraries/pico_display/pico_display.cmake)

set(TWIN_THING_PICO_CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/config)
set(COREMQTT_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/port/CoreMQTT)
set(COREMQTT_AGENT_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/port/CoreMQTT-Agent)


include(FreeRTOS_Kernel_import.cmake)
include(lwip_import.cmake)

include("lib/json-maker.cmake")
include("lib/tiny-json.cmake")
include("lib/twinThingPicoW/twinThingPicoW.cmake")
include("lib/FreeRTOS-coreMQTT.cmake")
include("lib/FreeRTOS-coreMQTT-Agent.cmake")


# Don't forget to link the libraries you need!
#target_link_libraries(${NAME}
#	pico_stdlib
#    pico_explorer # <-- List libraries here!
#)

# create map/bin/hex file etc.
#pico_add_extra_outputs(${NAME})

add_subdirectory(src)

#Set up files for the release packages
install(CODE "execute_process(COMMAND $ENV{HOME}/bin/picoDeploy.sh ${CMAKE_CURRENT_BINARY_DIR}/src/${NAME}.elf)")

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/${NAME}.uf2
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
