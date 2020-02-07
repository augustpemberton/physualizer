#define __AVR_AT90USB1286__
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "unifiedLcd.h"

void init(void);

int8_t enc_delta(void);
volatile int8_t delta;

#define TIMESCALE_STEP 0.1
float timescale = 1;

#define NUM_PARTICLES 3

#define __AVR_AT90USB1286__

#define STEP_DELAY 15
#define BALL_RADIUS 10
#define GRAVITY 9.8
#define GROUNDED_THRESHOLD 0.4
#define GROUNDED_EXIT_THRESHOLD 0.5

#define MIN_X 0
#define MIN_Y 0
#define MAX_X 320
#define MAX_Y 240

#define COEFF_REST 0.8
#define COEFF_FRIC 0.8

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  float radius;
  float mass;
  bool grounded;
} Particle;

Particle particles[NUM_PARTICLES];

int randRange(int min, int max) {
  return (rand() % (max + 1 - min) + min);
}

/*void drawBounds() {
  SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MAX_X, MIN_Y);
  SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MIN_X, MAX_Y);
  SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MAX_X, MIN_Y);
  SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MIN_X, MAX_Y);
}

void drawParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    if (particle->grounded) {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
    drawCircle(renderer, particle->position.x, particle->position.y, particle->radius);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void clearParticles() {
  SDL_RenderClear(renderer);
}
*/

void drawBounds() {
  drawLine(MIN_X, MIN_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MIN_X, MIN_Y, MIN_X, MAX_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MIN_X, MAX_Y, GRAY);
}

void drawParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    if (particle->grounded) {
      drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, RED);
    } else {
      drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, WHITE);
    }
  }
}

void clearParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, BLACK);
  }
}

void initializeParticles() {
  float scale;
  for (int i=0; i<NUM_PARTICLES; ++i) {
    scale = 1;
    particles[i].radius   = BALL_RADIUS * scale;

    particles[i].position = (Vector2){MIN_X + ((MAX_X-MIN_X) / NUM_PARTICLES) * i,
                                      MIN_Y + ((MAX_Y-MIN_Y) / NUM_PARTICLES) * i};
    particles[i].velocity = (Vector2){randRange(-50, 50), randRange(-50, 50)};

    particles[i].mass     = scale;
    particles[i].grounded = false;
  }
}

Vector2 gravityForce(Particle *particle) {
  return (Vector2){0, particle->mass * GRAVITY};
}

void applyForce(Particle *particle, Vector2 force, float dt) {
  Vector2 acceleration = (Vector2){force.x / particle->mass, force.y / particle->mass};
  particle->velocity.x += acceleration.x * dt;
  particle->velocity.y += acceleration.y * dt;
}

void applyContainerForce(Particle *particle) {
  if (particle->position.x - particle->radius < MIN_X) {
    particle->velocity.x = abs(particle->velocity.x) * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x += particle->radius - (particle->position.x - MIN_X );
  }
  if (particle->position.x + particle->radius > MAX_X) {
    particle->velocity.x = -1 * abs(particle->velocity.x) * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x -= particle->radius - (MAX_X - particle->position.x);
  }
  if (particle->position.y - particle->radius < MIN_Y) {
    particle->velocity.y = abs(particle->velocity.y) * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y += particle->radius - (particle->position.y - MIN_Y );
  }
  if (particle->position.y + particle->radius > MAX_Y) {
    particle->velocity.y = -1 * abs(particle->velocity.y) * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y -= particle->radius - (MAX_Y - particle->position.y);

    if (abs(particle->velocity.y) < GROUNDED_THRESHOLD && abs(particle->velocity.x) < GROUNDED_THRESHOLD) {
      particle->grounded = true;
    }
  }
}

void applyCollisions() {
  // iterate through each particle
  // check its not our particle
  // apply collision forces
  float distance;
  for (int j=0; j<NUM_PARTICLES; ++j) {
    Particle *particle = &particles[j];
    for (int i=j+1; i<NUM_PARTICLES; ++i) {
      Particle *particle2 = &particles[i];
      distance = sqrt(pow(particle->position.x - particle2->position.x, 2) + 
                      pow(particle->position.y - particle2->position.y, 2));
      if (distance < (particle->radius + particle2->radius)) {
        float dx = particle->position.x - particle2->position.x;
        float dy = particle->position.y - particle2->position.y;

        float distanceToMove = (particle->radius + particle2->radius) - sqrt(pow(dx,2) + pow(dy,2));

        float angle = atan(dy/dx);

        float dtx = distanceToMove * cos(angle);
        float dty = distanceToMove * sin(angle);

        particle->position = (Vector2){particle->position.x-abs(dtx), particle->position.y-abs(dty)};
        particle2->position = (Vector2){particle2->position.x+abs(dtx), particle2->position.y+abs(dty)};

        // A.v = (A.u * (A.m - B.m) + (2 * B.m * B.u)) / (A.m + B.m)
        // B.v = (B.u * (B.m - A.m) + (2 * A.m * A.u)) / (A.m + B.m)

        Vector2 au = particle->velocity;
        Vector2 bu = particle2->velocity;
        float k = particle->mass + particle2->mass;

        particle->velocity.x = (au.x * (particle->mass - particle2->mass) + (2 * particle2->mass * bu.x)) / k;
        particle->velocity.y = (au.y * (particle->mass - particle2->mass) + (2 * particle2->mass * bu.y)) / k;

        particle2->velocity.x = (bu.x * (particle2->mass - particle->mass) + (2 * particle->mass * au.x)) / k;
        particle2->velocity.y = (bu.y * (particle2->mass - particle->mass) + (2 * particle->mass * au.y)) / k;

        // check for grounded state
        if (particle->position.y < particle2->position.y) {
          if (abs(particle->velocity.y) < GROUNDED_THRESHOLD && abs(particle->velocity.x) < GROUNDED_THRESHOLD) {
            particle->grounded = true;
          }
        } else {
          if (abs(particle2->velocity.y) < GROUNDED_THRESHOLD && abs(particle2->velocity.x) < GROUNDED_THRESHOLD) {
            particle2->grounded = true;
          }
        }

      }
    }
  }
}

clock_t previousTime;
clock_t currentTime;
float deltaTime;

double getDT() {
  deltaTime = TCNT1;
  TCNT1 = 0;
  return deltaTime;
}

int main(void) {
    init();
    sei();

    float dt = 0;
    while (true) {
      //drawBounds();
      timescale -= TIMESCALE_STEP * enc_delta();
      dt = (getDT()* 0.00001) * timescale;

      clearParticles();
      applyCollisions();
      for (int i=0; i<NUM_PARTICLES; ++i) {
        Particle *particle = &particles[i];

        if (particle->grounded == true && abs(particle->velocity.y) > GROUNDED_EXIT_THRESHOLD) {
          particle->grounded = false;
        }

        if (particle->grounded) {
          particle->velocity.y = abs(particle->velocity.y);
        }

        if (particle->grounded == false) {
          applyForce(particle, gravityForce(particle), dt);
        }

        applyContainerForce(particle);
        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }

      drawParticles();
      _delay_ms(STEP_DELAY);
    }
    return 0;
}

void init(void) {

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
  initializeParticles();

  init_lcd(1);
}

ISR( TIMER0_COMPA_vect ) {
    static int8_t last;
    int8_t new, diff;
    uint8_t wheel;

    /*
    Scan rotary encoder
    ===================
    This is adapted from Peter Dannegger's code available at:
    http://www.mikrocontroller.net/articles/Drehgeber
    */

    wheel = PINE;
    new = 0;
    if( wheel  & _BV(PE4) ) new = 3;
    if( wheel  & _BV(PE5) )
    new ^= 1;                  /* convert gray to binary */
    diff = last - new;         /* difference last - new  */
    if( diff & 1 ){            /* bit 0 = value (1) */
        last = new;                /* store new as next last  */
        delta += (diff & 2) - 1;   /* bit 1 = direction (+/-) */
    }

}

/* read two step encoder */
int8_t enc_delta() {
    int8_t val;

    cli();
    val = delta;
    delta &= 1;
    sei();

    return val >> 1;
}