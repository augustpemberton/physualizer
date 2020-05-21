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

#include "rios.h"
#include "ruota.h"

const int MIN_X = 0;
const int MIN_Y = 0;
const int MAX_X = 318;
const int MAX_Y = 238;

int buttonForce = 100;
void check_switches();

void init(void) {
  initPhysics(MIN_X, MIN_Y, MAX_X, MAX_Y);
  initGraphics();
  srand(time(NULL));
  os_init_scheduler();
  os_init_ruota();
}

int main(void) {
    init();

    os_add_task(check_switches, 50, 1);
    // main physics loop        
    float dt = 0;
    Particle oldParticles[NUM_PARTICLES];
    while (true) {
      // store old particles so we can remove them later
      memcpy(oldParticles, particles, sizeof(oldParticles));
      waitForNextFrame();

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
        dt = getDT();
        Particle *particle = &particles[i];
        applyGravity(particle, dt);

        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }

      redrawParticles(oldParticles, particles, NUM_PARTICLES);
      drawForce(buttonForce);

    }
    return 0;
}

void check_switches() {
  if (get_switch_press(_BV(SWN))) {
    for (int i=0; i<NUM_PARTICLES; ++i) {
      particles[i].velocity.y -= buttonForce;
    }
  }
  if (get_switch_press(_BV(SWS))) {
    for (int i=0; i<NUM_PARTICLES; ++i) {
      particles[i].velocity.y += buttonForce;
    }
  }
  if (get_switch_press(_BV(SWE))) {
    for (int i=0; i<NUM_PARTICLES; ++i) {
      particles[i].velocity.x += buttonForce;
    }
  }
  if (get_switch_press(_BV(SWW))) {
    for (int i=0; i<NUM_PARTICLES; ++i) {
      particles[i].velocity.x -= buttonForce;
    }
  }
  buttonForce += os_enc_delta() * 5;
}