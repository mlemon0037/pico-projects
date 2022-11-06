add_executable(blow
blow.c
)

pico_enable_stdio_usb(blow 1)
pico_add_extra_outputs(blow)
target_link_libraries(blow pico_stdlib hardware_adc hardware_dma)