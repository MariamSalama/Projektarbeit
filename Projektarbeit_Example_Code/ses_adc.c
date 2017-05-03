/*INCLUDES *******************************************************************/

#include "ses_common.h"
#include "ses_adc.h"
#include <math.h>

/* DEFINES & MACROS **********************************************************/

// ADC wiring on SES board
#define ADC_PORT       			PORTF
#define TEMP_SENSOR_PIN         2
#define LIGHT_SENSOR_PIN        4
#define JOYSTICK_SENSOR_PIN   	5

#define ADC_MUX_MASK			0b11100000
#define PRESCALE_CLEAR_MASK		0b11111000

#define JOYSTICK_MARGIN			100
#define JOYSTICK_RIGHT			200
#define JOYSTICK_UP				400
#define JOYSTICK_LEFT			600
#define JOYSTICK_DOWN			800

//Values and details for this can be found in the sheet for the third exercise
#define ADC_TEMP_MAX			40
#define ADC_TEMP_MIN			10
#define ADC_TEMP_R_T_MAX		exp(3250 / (ADC_TEMP_MAX + 273.15) - 3250 / 298.15) * 10000
#define ADC_TEMP_R_T_MIN		exp(3250 / (ADC_TEMP_MIN + 273.15) - 3250 / 298.15) * 10000
#define ADC_TEMP_RAW_MAX		5 * (ADC_TEMP_R_T_MAX / (ADC_TEMP_R_T_MAX + 68000))
#define ADC_TEMP_RAW_MIN		5 * (ADC_TEMP_R_T_MIN / (ADC_TEMP_R_T_MIN + 68000))
#define	ADC_TEMP_FACTOR			10

//microphone pins


/* FUNCTION DEFINITION *******************************************************/

void adc_init(void){
	//Set DDR registers to set the pins as input
	DDR_REGISTER(PORTF) &= ~(1<<TEMP_SENSOR_PIN);
	DDR_REGISTER(PORTF) &= ~(1<<LIGHT_SENSOR_PIN);
	DDR_REGISTER(PORTF) &= ~(1<<JOYSTICK_SENSOR_PIN);

	//enable internal pullups
	ADC_PORT |= 1<<TEMP_SENSOR_PIN;
	ADC_PORT |= 1<<LIGHT_SENSOR_PIN;
	ADC_PORT |= 1<<JOYSTICK_SENSOR_PIN;

	//disable power reduction mode (enabling the adc)
	PRR0 &= ~(1<<PRADC);
	//configure voltage reference
	ADMUX |= (ADC_VREF_SRC<<REFS0);
	//clear ADLAR bit to set the ADC result right adjusted
	ADMUX &= ~(1<<ADLAR);
	//set the prescaler
	ADCSRA &= PRESCALE_CLEAR_MASK;
	ADCSRA |= ADC_PRESCALE;
	//disable auto trigger
	ADCSRA &= ~(1<<ADATE);
	//enable the ADC
	ADCSRA |= 1<<ADEN;
}


uint16_t adc_read(uint8_t adc_channel){
	//check if input is valid
	if(adc_channel>=ADC_NUM)
		return ADC_INVALID_CHANNEL;

	//configure the channel
	ADCSRB &= ~(1<<MUX5);
	ADMUX &= ADC_MUX_MASK;
	ADMUX |= (adc_channel);

	//start conversion
	ADCSRA |= 1<<ADSC;

	//wait for conversion to end
	while(ADCSRA & (1<<ADSC)); //TODO Should run parallel (use interrupt)

	return ADC;
}

//Checks the current orientation of the joystick and returns it
uint8_t adc_getJoystickDirection(void){
	uint16_t joystickvalue = adc_read(ADC_JOYSTICK_CH);
	if ((joystickvalue <= (JOYSTICK_RIGHT + JOYSTICK_MARGIN))
			&& (joystickvalue >= (JOYSTICK_RIGHT - JOYSTICK_MARGIN)))
		return RIGHT;
	else if ((joystickvalue <= (JOYSTICK_UP + JOYSTICK_MARGIN))
			&& (joystickvalue >= (JOYSTICK_UP - JOYSTICK_MARGIN)))
		return UP;
	else if ((joystickvalue <= (JOYSTICK_LEFT + JOYSTICK_MARGIN))
			&& (joystickvalue >= (JOYSTICK_LEFT - JOYSTICK_MARGIN)))
		return LEFT;
	else if ((joystickvalue <= (JOYSTICK_DOWN + JOYSTICK_MARGIN))
			&& (joystickvalue >= (JOYSTICK_DOWN - JOYSTICK_MARGIN)))
		return DOWN;
	else
		return NO_DIRECTION;
}

//Returns the current measured temperature, due to performance reasons
//this is done by linear interpolation instead of exact calculation
int16_t adc_getTemperature(void){
	//read raw adc value in volts
	int16_t adc = adc_read(ADC_TEMP_CH);

	//Calculate temperature by linear interpolation
	int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
	int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
	return (adc * slope + offset) / ADC_TEMP_FACTOR;
}

