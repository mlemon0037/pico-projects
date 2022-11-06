// idea yoinked from this guy vvv
// https://hackaday.com/2018/08/21/an-led-you-can-blow-out-with-no-added-sensor/
// https://github.com/paulhdietz/LEDSensors/blob/master/_07_BlowOutLED/_07_BlowOutLED.ino

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define PLUS 15                 // digital output to led
#define ADC_PIN 26              // reads analog voltage between led and resistor

#define THRESHOLD 3             // threshold in voltage difference to turn off led
#define ADC_COUNT 512           // number of adc readings to average 
#define REF_COUNT ADC_COUNT*2   // number of adc readings to average for the reference

#define CONVERSION_FACTOR (3.3 / (1 << 12)) // number from documentation to convert adc output to voltage

#define CALLBACK_TIME 10000     // amount of time to wait bewteen recalculating reference

// reads ADC_COUNT voltages from the adc and averages them
void analog_read(int *var){
    *var = 0; // set parameter to 0 so it can hold sum temporarily
    for (int i = 0; i < ADC_COUNT; i++){
        *var += adc_read(); // sum ADC_COUNT readings
    }
    *var /= ADC_COUNT; // divide by sum of samples to get average
}

// same as analog_read but more samples, should probably just be one function but idc
void recalculateReference(int *reference){ 
    *reference = 0;
    for (int i = 0; i < REF_COUNT; i++){
        *reference += adc_read();
    }
    *reference /= REF_COUNT;
}

// callback function that is called every CALLBACK_TIME ms
bool repeating_timer_callback(struct repeating_timer *t){
    // recalulate reference voltage with the user data in timer, cast void* to int* to get meaningful data
    recalculateReference((int*)t->user_data);
    return true;
}

int main() {

    // initialize GPIO and adc
    stdio_init_all();
    gpio_init(PLUS);
    gpio_set_dir(PLUS, GPIO_OUT);
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    // start LED
    gpio_put(PLUS, 1);
    int voltage_reference = 0;
    int voltage_reading = 0;
    recalculateReference(&voltage_reference);

    // create struct for the timer
    struct repeating_timer timer;
    // add our callback function to run every CALLBACK_TIME ms and give the callback our voltage ref pointer
    add_repeating_timer_ms(CALLBACK_TIME, repeating_timer_callback, &voltage_reference, &timer);

    while(1) {        

        // get current voltage from the ADC
        analog_read(&voltage_reading);
        printf("Raw Reading %fV; Reference: %fV, Diff: %fV\n", 
            voltage_reading*CONVERSION_FACTOR, 
            voltage_reference*CONVERSION_FACTOR, 
            (voltage_reference - voltage_reading)*CONVERSION_FACTOR
            );


        // if the difference in our reference-reading is hits the threshold, turn off led and recalc reference
        if (voltage_reference - voltage_reading >= THRESHOLD){

            gpio_put(PLUS, 0);  // we are below threshold, so turn off
            sleep_ms(1000);     // delay so a person can see it
            gpio_put(PLUS, 1);  // turn led back on

            recalculateReference(&voltage_reference);
        }
    }
}

