/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include "ses_bits.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC 	1249
#define TIMER2_CYC_FOR_1MILLISEC 	249

/* CALLBACKS **********************************************************/

static pTimerCallback timer1Callback = NULL;
static pTimerCallback timer2Callback = NULL;

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	timer2Callback = cb;
}

void timer2_init() {

	//make sure the timer is stopped
	timer2_stop();

	//Enable CTC
	CLEAR_BIT(TCCR2A, WGM20);
	SET_BIT(TCCR2A, WGM21);
	CLEAR_BIT(TCCR2B, WGM22);

	//Compare Match A Interrupt Enable
	SET_BIT(TIMSK2, OCIE2A);

	//Clear Interrupt Flag
	SET_BIT(TIFR2, OCF2A);

	//Set maxValue that defines when the timer triggers
	OCR2A = TIMER2_CYC_FOR_1MILLISEC; //((16*10^6) * Prescaler) / desired_interrupts_per_sec
}

void timer2_start() {
	//set prescaler to 64
	CLEAR_BIT(TCCR2B, CS20);
	CLEAR_BIT(TCCR2B, CS21);
	SET_BIT(TCCR2B, CS22);
}

void timer2_stop() {
	//set prescaler to 0
	CLEAR_BIT(TCCR2B, CS20);
	CLEAR_BIT(TCCR2B, CS21);
	CLEAR_BIT(TCCR2B, CS22);
}

void timer1_setCallback(pTimerCallback cb) {
	timer1Callback = cb;
}

void timer1_init() {

	//make sure timer is stopped
	timer1_stop();

	//Enable CTC
	CLEAR_BIT(TCCR1A, WGM10);
	CLEAR_BIT(TCCR1A, WGM11);
	SET_BIT(TCCR1B, WGM12);
	CLEAR_BIT(TCCR1B, WGM13);

	//Compare Match A Interrupt Enable
	SET_BIT(TIMSK1, OCIE1A);

	//Clear Interrupt Flag
	SET_BIT(TIFR1, OCF1A);

	//Set maxValue that defines when the timer triggers
	OCR1A = TIMER1_CYC_FOR_5MILLISEC; //((16*10^6) / Prescaler) / desired_interrupts_per_sec
}

void timer1_start() {
	//set prescaler to 64
	SET_BIT(TCCR1B, CS10);
	SET_BIT(TCCR1B, CS11);
	CLEAR_BIT(TCCR1B, CS12);
}

void timer1_stop() {
	//set prescaler to 0
	CLEAR_BIT(TCCR1B, CS10);
	CLEAR_BIT(TCCR1B, CS11);
	CLEAR_BIT(TCCR1B, CS12);
}

ISR(TIMER1_COMPA_vect) {
	if (timer1Callback != NULL) {
		timer1Callback();
	}
}

ISR(TIMER2_COMPA_vect) {
	if (timer2Callback != NULL) {
		timer2Callback();
	}
}
