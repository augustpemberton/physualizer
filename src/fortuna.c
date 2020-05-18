#define __AVR_AT90USB1286__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "fortuna.h"

void initFortuna() {

  /* 8MHz clock, no prescaling (DS, p. 48) */
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;

  /* Configure I/O Ports */
  DDRB  |=  _BV(PB7);   /* LED pin out */
  PORTB &= ~_BV(PB7);   /* LED off */

  /* Timer 3 for physics engine */
  TCCR3B |= (1 << CS20);

  srand(time(NULL));
  init_lcd(1);
}

float getTimer() {
  float dt = TCNT3;
  return dt;
}

void resetTimer() {
  TCNT3 = 0;
}

float getTimerDelta() {
  float dt = getTimer();
  resetTimer();
  return dt;
}
