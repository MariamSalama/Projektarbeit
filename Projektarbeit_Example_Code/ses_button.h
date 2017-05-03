#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

//Function pointer to be used in button callbacks
typedef void (*pButtonCallback)();

/**
 * setter function for providing a executable function on rotary button push
 */
void button_setRotaryButtonCallback(pButtonCallback callback);

/**
 * setter function for providing a executable function on joystick button push
 */
void button_setJoystickButtonCallback(pButtonCallback callback);

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool debouncing);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

/**
 * Sets the function that is called when the rotary button is pressed
 * @param callback the function to be called (needs to be passed without brackets "()")
 */
void button_setRotaryButtonCallback(pButtonCallback callback);

/**
 * This function is used for debouncing 8 buttons at the same time
 */
void button_checkState(void);

#endif /* SES_BUTTON_H_ */
