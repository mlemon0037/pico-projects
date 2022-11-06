add_executable(ultrasonic
ultrasonic.c
)

pico_enable_stdio_usb(ultrasonic 1)
pico_add_extra_outputs(ultrasonic)
target_link_libraries(ultrasonic pico_stdlib)
