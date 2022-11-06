#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico.h"
#include "hardware/pwm.h"

// pin mapping
#define IN1 14
#define IN2 15
#define IN3 16
#define IN4 17

#define LEFT_SENSOR 18
#define RIGHT_SENSOR 19

enum states{
	forward = 0,
	backward_left,
	turn_left,
	backward_right,
	turn_right
};


enum states state = forward;


// function definitions
void turnLeft();
void turnRight();
void driveForward();
void driveBackward();

int main(int argc, char const *argv[])
{
	// initialize I/O and set GPIO directions
	gpio_init(IN1);
	gpio_init(IN2);
	gpio_init(IN3);
	gpio_init(IN4);

	gpio_set_dir(IN1, GPIO_OUT);
	gpio_set_dir(IN2, GPIO_OUT);
	gpio_set_dir(IN3, GPIO_OUT);
	gpio_set_dir(IN4, GPIO_OUT);


	gpio_init(LEFT_SENSOR);
	gpio_init(RIGHT_SENSOR);

	gpio_set_dir(LEFT_SENSOR, GPIO_IN);
	gpio_set_dir(RIGHT_SENSOR, GPIO_IN);

	// predefine variables to store sensor readings
	int left_reading;
	int right_reading;

	driveForward();

	while (1) {
		// if object is too close, sensor outputs 0, otherwise 1
		// invert to make logic easier
		left_reading = !gpio_get(LEFT_SENSOR);
		right_reading = !gpio_get(RIGHT_SENSOR);


		if (state == forward) {
			if (left_reading || right_reading) { // if we trip a sensor
				driveBackward(); // move backward

				if (left_reading) // decide which state to move to
					state = backward_left;
				else
					state = backward_right;
			} else { // if no sensor is tripped, just drive forward
				driveForward();
			}

		} else if ((state == backward_left) || (state == backward_right)) {
			if (!left_reading && !right_reading) { // once we are no longer tripping the sensor
				if (state == backward_right) {
					turnLeft();
					state = turn_left;
				} else {
					turnRight();
					state = turn_right;
				}
			}

		} else if (state == turn_left) {
			state = forward;

		} else if (state == turn_right) {
			state = forward;
		}

	}

	return 0;
}

void left_wheel_forward(){
	gpio_put(IN1, 1);
	gpio_put(IN2, 0);
}

void left_wheel_backward(){
	gpio_put(IN1, 0);
	gpio_put(IN2, 1);	
}

void right_wheel_forward(){
	gpio_put(IN3, 0);
	gpio_put(IN4, 1);
}

void right_wheel_backward(){
	gpio_put(IN3, 1);
	gpio_put(IN4, 0);
}

// various functions to turn the robot
void turnRight() {
	// turn to the right
	left_wheel_forward();
	right_wheel_backward();
}

void turnLeft() {
	// turn to the left
	left_wheel_backward();
	right_wheel_forward();
}

void driveForward() {
	left_wheel_forward();
	right_wheel_forward();
}

void driveBackward(){
	left_wheel_backward();
	right_wheel_backward();
}