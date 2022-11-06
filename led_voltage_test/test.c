/*  this is kind of jank but it works for some reason
    final version should probs use photodiode but an led works
    uses the adc to read the voltage the LED is generating due to light
    the brighter the light, the lower the reading is, capped at 0
    has been tested in reasonably high light, and can easily detect phone flashlight
    full idea is to mount 3 of these on the front of a robot about 45 degress apart
    if the front led has lowest value, go forward
    if the right led has the lowest value, turn left
    if the left led has the lowest value, turn right

    if they are all similar, it chooses the front one
    this is behavior that i wanted, but did not code it, it just happened

    the robot should turn to find the brightest light, then follow it
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/malloc.h"
#include "hardware/adc.h"


// just an enum with each led
enum led{
    left = 0,
    middle,
    right,
    COUNT
};

#define LEFT_LED 26      // connect anode of LED to pin 26 and cathode to GND (ik its weird but it works better)
#define MIDDLE_LED 27    // do the same for middle and right
#define RIGHT_LED 28

#define THRESHOLD 250

#define CONVERSION_FACTOR (3.3 / (1 << 12)) // funny number from documentation to convert adc output to voltage

// function that returns the direction of the brightest light
enum led lightDirection(){
    enum led brightest_index = middle;    // default to straight


    adc_select_input(left);
    uint16_t left_val = adc_read();
    adc_select_input(right);
    uint16_t right_val = adc_read();

    int diff = right_val - left_val;

    printf("Left:%d, Right: %d, Diff: %d\n", left_val, right_val, diff);

    return 0;
}

// enum led lightDirection() {
//     adc_select_input(0);
//     int left = adc_read();
//     adc_select_input(1);
//     int middle = adc_read();
//     adc_select_input(2);
//     int right = adc_read();
//     printf("Left: %d; Middle: %d; Right: %d\n", left, middle, right);
//     sleep_ms(10);
//     return -1;
// }

int main()
{
    // initializing stuff
    // initialize io for printf for debugging
	stdio_init_all();
    // initialize the adc
	adc_init();
    // put pins 26, 27, 28 into adc mode
    adc_gpio_init(LEFT_LED);
    gpio_disable_pulls(LEFT_LED);
    adc_gpio_init(MIDDLE_LED);
    gpio_disable_pulls(MIDDLE_LED);
    adc_gpio_init(RIGHT_LED);
    gpio_disable_pulls(RIGHT_LED);

    // there is only one adc, so we need to mux the 3 adc inputs
    // the mask determines what adc inputs we round robin through
    // 0x7 = 0b0111
    // bit 0 in mask = pin 26 enabled for adc round robin,
    // bit 1 enables pin 27 for round robin, bit 2 enable pin 28 for round robin
    // so subsequent calls to adc_read will auto go adc0->adc1->adc2->adc0->adc1->...
    // allows us to use multiple adc inputs without switching with adc_select_input every read
    // adc_set_round_robin(0x7);

    // init buffer for proof of concept debugging
    char *buf = (char *)malloc(25);



    // get the direction of the brightest light
    enum led brightest = lightDirection();
    while (1){
        // printf(buf);
        // recalc brightest light direction
        brightest = lightDirection();
    }
}
