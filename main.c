#define FORTUNA true

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#if !FORTUNA
# include "src/graphics.h"
#else
# include "src/graphicsF.h"
#endif

#include "src/physics.h"

float timescale = 1.5;
#define TIMESCALE_STEP 0.0003;

const int MIN_X = 0;
const int MIN_Y = 0;
const int MAX_X = 318;
const int MAX_Y = 238;

Vector2 buttonForce = {100,100};

void init(void) {
  initPhysics(MIN_X, MIN_Y, MAX_X, MAX_Y);
  initGraphics();
  srand(time(NULL));
}

int main(void) {
    init();
    float dt = 0;
    Particle oldParticles[NUM_PARTICLES];
    while (userQuit == 0) {
      memcpy(oldParticles, particles, sizeof(oldParticles));
      waitForNextFrame();
      //drawBounds();

      // apply collisions 
      for (int i=0; i<1; i++) {
        applyCollisions();
        for (int p=0; p<NUM_PARTICLES; ++p) {
          Particle *particle = &particles[p];
          applyContainerForce(particle);
        }
      }

      // apply forces and draw
      for (int i=0; i<NUM_PARTICLES; ++i) {
        dt = getDT() * timescale;
        Particle *particle = &particles[i];
        applyGravity(particle, dt);


        /*
        if (buttonPressed) {
          applyForce(particle, (Vector2){0, -30}, dt);
        }
        if (speedUpPressed) {
          timescale += TIMESCALE_STEP;
        }

        if (slowDownPressed) {
          timescale -= TIMESCALE_STEP;
        }
        */

        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }

      redrawParticles(oldParticles, particles, NUM_PARTICLES);

    }
    return 0;
}

ISR( INT7_VECT ) {
  for (int i=0; i<NUM_PARTICLES; ++i) {
    particles[i].velocity.x += buttonForce.x;
    particles[i].velocity.y += buttonForce.y;
  }
}
