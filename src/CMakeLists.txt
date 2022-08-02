add_executable(${NAME}
        main.cpp
        BlinkPicoWTask.cpp
        ExampleAgentObserver.cpp
        StateExample.cpp
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} 
	 pico_stdlib
     FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
     FREERTOS_PORT
     LWIP_PORT
     pico_cyw43_arch_lwip_sys_freertos
	 pico_unique_id
	 twinThingPicoW 
	 json_maker 
	 tiny_json
	 hardware_rtc
	 coreMQTT 
	 coreMQTTAgent
	)
	
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
     )
     
target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID=\"$ENV{WIFI_SSID}\"
    WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
    configNUM_CORES=2
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# enable usb output, disable uart output
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)