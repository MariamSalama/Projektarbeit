/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_button.h"
#include "ses_bits.h"
#include "ses_timer.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// Button Pins on SES board
#define ENCODER_BUTTON_PORT       	PORTB
#define ROTARY_BUTTON_PIN         	6

#define JOYSTICK_BUTTON_PORT       	PORTB
#define JOYSTICK_BUTTON_PIN         7

//Number of debounce checks
#define BUTTON_NUM_DEBOUNCE_CHECKS	5

/* VARIABLES */
static bool rotaryButtonStatus = false, joystickButtonStatus = false;
static pButtonCallback rotaryCallback = NULL;
static pButtonCallback joystickCallback = NULL;

/* FUNCTION DEFINITION *******************************************************/

void button_init(bool debouncing) {
	//Enable buttons
	DDR_REGISTER(ENCODER_BUTTON_PORT) &= ~(1 << ROTARY_BUTTON_PIN);
	DDR_REGISTER(JOYSTICK_BUTTON_PORT) &= ~(1 << JOYSTICK_BUTTON_PIN);
	ENCODER_BUTTON_PORT |= 1 << ROTARY_BUTTON_PIN;
	JOYSTICK_BUTTON_PORT |= 1 << JOYSTICK_BUTTON_PIN;

	if (debouncing) {
		timer1_init();
		timer1_setCallback(&button_checkState);
		timer1_start();
	} else {
		//remove comments on code below to enable direct interrupts
//		Enable interrupts 8:0

//		PCICR |= 1 << PCIE0;
//		Enable interrupts for rotary button and joystick button
//		PCMSK0 |= 1 << ROTARY_BUTTON_PIN;
//		PCMSK0 |= 1 << JOYSTICK_BUTTON_PIN;
	}
}

bool button_isJoystickPressed(void) {
	return ((PIN_REGISTER(JOYSTICK_BUTTON_PORT) & (1 << JOYSTICK_BUTTON_PIN))
			== 0);
}

bool button_isRotaryPressed(void) {
	return ((PIN_REGISTER(ENCODER_BUTTON_PORT) & (1 << ROTARY_BUTTON_PIN)) == 0);
}

void button_setRotaryButtonCallback(pButtonCallback callback) {
	rotaryCallback = callback;
}

void button_setJoystickButtonCallback(pButtonCallback callback) {
	joystickCallback = callback;
}

ISR(PCINT0_vect) {
	//Check which button was pressed
	if (button_isRotaryPressed()) {
		//Check if button was not pressed before
		if (!rotaryButtonStatus) {
			rotaryButtonStatus = 1;
			//Check whether mask register is set
			if (BIT_IS_SET(PCMSK0, ROTARY_BUTTON_PIN)) {
				//Check whether a valid callback function was set
				if (rotaryCallback != NULL) {
					rotaryCallback();
				}
			}
		}
		//if button was pressed reset the status
	} else {
		rotaryButtonStatus = 0;
	}

	if (button_isJoystickPressed()) {
		if (!joystickButtonStatus) {
			joystickButtonStatus = 1;
			//Check whether mask register is set
			if (BIT_IS_SET(PCMSK0, JOYSTICK_BUTTON_PIN)) {
				//Check whether a valid callback function was set
				if (joystickCallback != NULL) {
					joystickCallback();
				}
			}
		}
	} else {
		joystickButtonStatus = 0;
	}
}

void button_checkState() {
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;

	// each bit in every state byte represents one button
	state[index] = 0;
	if (button_isJoystickPressed()) {
		state[index] |= 1;
	}
	if (button_isRotaryPressed()) {
		state[index] |= 2;
	}

	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}

	/**
	 * initialize compare value and compare with ALL reads, only if
	 * we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "0" in the state
	 * array, the button at this position is considered pressed
	 */
	uint8_t j = 0xFF;
	for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i]; //What does this check against for first few iterations ?
	}
	debouncedState = j;

	/**
	 * The callback is only supposed to be called when the
	 * button is pressed for and held for the first time
	 * therefore only when the last state was not set
	 * and the current one is set it should be called
	 */
	if (!BIT_IS_SET(lastDebouncedState, 0) && BIT_IS_SET(debouncedState, 0)) {
		if (joystickCallback != NULL) {
			joystickCallback();
		}
	}
	if (!BIT_IS_SET(lastDebouncedState, 1) && BIT_IS_SET(debouncedState, 1)) {
		if (rotaryCallback != NULL) {
			rotaryCallback();
		}
	}
}

