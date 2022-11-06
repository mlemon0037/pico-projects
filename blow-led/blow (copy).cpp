// idea yoinked from this guy vvv
// https://hackaday.com/2018/08/21/an-led-you-can-blow-out-with-no-added-sensor/
// https://github.com/paulhdietz/LEDSensors/blob/master/_07_BlowOutLED/_07_BlowOutLED.ino

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define PLUS 22     // providing voltage to the circuit
#define ADC_PIN 26  // reads votltage between led and resistor

#define THRESHOLD 2
#define ADC_COUNT 1024
#define AVERAGE_MULTIPLIER 4

#define CONVERSION_FACTOR (3.3 / (1 << 12))


int recalculateReference(){
    int sum = 0;
    int i;
    for (i = 0; i < ADC_COUNT*AVERAGE_MULTIPLIER; ++i){
        sum += adc_read();
    }
    return sum / (ADC_COUNT*AVERAGE_MULTIPLIER);
}



int getADCReading(){
    int sum = 0;

    for (int i = 0; i < ADC_COUNT; i++){
        sum += adc_read(); // this may be its
    }
    return sum / ADC_COUNT;
}

bool repeating_timer_callback(struct repeating_timer *t){

    *((int *)t->user_data) = recalculateReference();
    printf("Recalculated Reference to %d", *((int *)t->user_data));

    return true;
}


int main() {

    stdio_init_all();

    // initialize GPIO and adc
    gpio_init(PLUS);
    gpio_set_dir(PLUS, GPIO_OUT);
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    // start LED
    gpio_put(PLUS, 1);
    int voltage_reference = recalculateReference();

    struct repeating_timer timer;
    add_repeating_timer_ms(10000, repeating_timer_callback, &voltage_reference, &timer);

    while(1) {        

        sleep_ms(10);
        int average_voltage_reading = getADCReading();
        printf("Raw Reading %fV; Reference: %fV, Diff: %fV\n", 
            average_voltage_reading*CONVERSION_FACTOR, 
            voltage_reference*CONVERSION_FACTOR, 
            (voltage_reference - average_voltage_reading)*CONVERSION_FACTOR
            );


        if (voltage_reference - average_voltage_reading >= THRESHOLD){

            gpio_put(PLUS, 0);  // we are below threshold, so turn off
            sleep_ms(1000);     // delay so a person can see it
            gpio_put(PLUS, 1);  // turn led back on

            voltage_reference = recalculateReference();
        }
    }
}

