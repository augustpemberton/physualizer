/*
 * Plays a song given the note pitches and durations using PWM
 * Adapted from:
 * https://github.com/fatcookies
 * and
 * http://avrpcm.blogspot.co.uk/2010/11/playing-8-bit-pcm-using-any-avr.html 
 */

#include <stdint.h>

void pwm_init();
void click();