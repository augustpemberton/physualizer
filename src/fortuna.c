#define __AVR_AT90USB1286__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#include "fortuna.h"

void initFortuna() {

  /* 8MHz clock, no prescaling (DS, p. 48) */
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;

  /* Configure I/O Ports */
  DDRB  |=  _BV(PB7);   /* LED pin out */
  PORTB &= ~_BV(PB7);   /* LED off */


  DDRE &= ~_BV(PE4) & ~_BV(PE5);  /* Rot. Encoder inputs */
  PORTE |= _BV(PE4) | _BV(PE5);   /* Rot. Encoder pull-ups */


  /* Timer 0 for switch scan interrupt: */
  TCCR0A = _BV(WGM01);  /* CTC Mode, DS Table 14-7 */
  TCCR0B = _BV(CS01)
          | _BV(CS00);   /* Prescaler: F_CPU / 64, DS Table 14-8 */

  /* Timer 1 for physics engine */
  TCCR1B |= (1 << CS10);

  /* 1 ms for manual movement of rotary encoder: */
  /* 1 ms --> 1000 Hz, Formula for CTC mode from  DS 14.6.2  */
  /* Note that the formula gives the toggle frequency, which is half the interrupt frequency. */

  OCR0A = (uint8_t)(F_CPU / (64.0 * 1000) - 1);

  TIMSK0 |= _BV(OCIE0A);  /* Enable timer interrupt, DS 14.8.6  */

  srand(time(NULL));
  init_lcd(1);
}

float getTimerDelta() {
  float dt = TCNT1;
  TCNT1 = 0;
  return dt;
}