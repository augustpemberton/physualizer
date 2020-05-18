/*
 * Plays a song given the note pitches and durations using PWM
 * Adapted from:
 * https://github.com/fatcookies
 * and
 * http://avrpcm.blogspot.co.uk/2010/11/playing-8-bit-pcm-using-any-avr.html 
 */


#include "audio.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>


/* initialise the PWM */
void pwm_init() {
	 
   /* use OC1A (RCH) and OC3A (LCH) pin as output */
    DDRB |= _BV(PB5);

		TCCR1A |= _BV(COM1A1) | _BV(WGM10);
    TCCR1B |= _BV(WGM12) | _BV(CS10);
    
    /* set initial duty cycle to zero */
    OCR1A = 200;
}

void click() {
	OCR1A = 10000;
}
