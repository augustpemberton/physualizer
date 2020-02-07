#define FORTUNA false

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#if !FORTUNA
# include "src/graphics.h"
#else
# include "src/graphicsF.h"
#endif

#include "src/physics.h"

float timescale = 1;

const int MIN_X = 0;
const int MIN_Y = 0;
const int MAX_X = 640;
const int MAX_Y = 480;

void init(void) {
  initPhysics(MIN_X, MIN_Y, MAX_X, MAX_Y);
  initGraphics(MAX_X, MAX_Y);
  srand(time(NULL));
}

int main(void) {
    init();
    float dt = 0;
    while (userQuit == 0) {
      waitForNextFrame();
      //drawBounds();
      dt = getDT() * timescale;

      clearParticles(particles, NUM_PARTICLES);
      for (int i=0; i<50; i++) {
        applyCollisions();
        for (int p=0; p<NUM_PARTICLES; ++p) {
          Particle *particle = &particles[i];
          applyContainerForce(particle);
        }
      }
      for (int i=0; i<NUM_PARTICLES; ++i) {
        Particle *particle = &particles[i];
        //applyContainerForce(particle);
        applyGravity(particle, dt);

        if (buttonPressed) {
          applyForce(particle, (Vector2){100, -100}, dt);
        }
        //dampenVelocity(particle, dt);
        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }

      drawParticles(particles, NUM_PARTICLES);
      

    }
    return 0;
}

