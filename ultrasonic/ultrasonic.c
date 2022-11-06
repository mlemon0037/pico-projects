
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico.h"
#include "hardware/timer.h"

#define ECHO 15
#define TRIG 14

#define TIME_TO_CM 0.034 / 2
#define CM_TO_IN 0.394


int main(){
	stdio_init_all();
    gpio_init(TRIG);
    gpio_set_dir(TRIG, GPIO_OUT);
    gpio_init(ECHO);
    gpio_set_dir(ECHO, GPIO_IN);

    uint64_t t_elapsed;
	uint64_t t_start;
	uint64_t t_end;
    sleep_ms(1000);

    while (1){
    	gpio_put(TRIG, 0);
    	sleep_ms(100);
    	gpio_put(TRIG, 1);
    	sleep_us(2);
    	gpio_put(TRIG, 0);

    	while (gpio_get(ECHO) == 0){
    		printf("please\n");
    		t_start = time_us_64();
    	}
    	while (gpio_get(ECHO) == 1){
    		t_end = time_us_64();
    		printf("LETS FUCKING GOOOOOOO\n");
    	}

    	t_elapsed = t_end-t_start;

    	printf("%fcm or %fin\n", t_elapsed*TIME_TO_CM, t_elapsed*TIME_TO_CM*CM_TO_IN);
    	sleep_ms(1000);
    }
}