#define FORTUNA true

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// SDL
//#if !FORTUNA
# include "src/graphics.h"
//#else
# include "src/unifiedLcd.h"
//#endif

#include "src/physics.h"

int8_t enc_delta(void);
volatile int8_t delta;

#define TIMESCALE_STEP 0.2
float timescale = 1;

#define STEP_DELAY 0

#define MIN_X 0
#define MIN_Y 0
#define MAX_X 320
#define MAX_Y 240

int randRange(int min, int max) {
  return (rand() % (max + 1 - min) + min);
}

void init(void) {
  if (FORTUNA) {
    initFortuna();
  } else {
    initWindow();
    previousTime = clock();
    currentTime = clock();
  }

  srand(time(NULL));
  initializeParticles();
}

void drawBounds() {
  if (FORTUNA) {
    drawLine(MIN_X, MIN_Y, MAX_X, MIN_Y, GRAY);
    drawLine(MIN_X, MIN_Y, MIN_X, MAX_Y, GRAY);
    drawLine(MAX_X, MAX_Y, MAX_X, MIN_Y, GRAY);
    drawLine(MAX_X, MAX_Y, MIN_X, MAX_Y, GRAY);
  } else {
    SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MAX_X, MIN_Y);
    SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MIN_X, MAX_Y);
    SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MAX_X, MIN_Y);
    SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MIN_X, MAX_Y);
  }
}

void drawParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    if (FORTUNA){

      for (int i=0; i < NUM_PARTICLES; ++i) {
        Particle *particle = &particles[i];
        if (particle->grounded) {
          drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, RED);
        } else {
          drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, WHITE);
        }
      }

    } else {

      if (particle->grounded) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      }


    drawCircle(renderer, particle->position.x, particle->position.y, particle->radius);
    }
  }

  if (!FORTUNA){ 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  }
}

void clearParticles() {
  if (FORTUNA) {
    for (int i=0; i < NUM_PARTICLES; ++i) {
      Particle *particle = &particles[i];
      drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, BLACK);
    }
  } else {
    SDL_RenderClear(renderer);
  }
}

clock_t previousTime;
clock_t currentTime;
float deltaTime;
double getDT() {
  if (FORTUNA) {
    deltaTime = getTimerDelta();
  } else {
    currentTime = clock();
    deltaTime = (double) (currentTime - previousTime);
    previousTime = currentTime;
  }

  return deltaTime;
}

int main(void) {
    init();
    float dt = 0;
    while (userQuit == 0) {
      //drawBounds();
      dt = (getDT() / 100000) * timescale;

      clearParticles();

      applyCollisions();
      for (int i=0; i<NUM_PARTICLES; ++i) {
        Particle *particle = &particles[i];
        applyContainerForce(particle);
        applyGravity(particle, dt);
        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }

      drawParticles();
      
      if (FORTUNA) {
        _delay_ms(STEP_DELAY);
      } else {
        handleEvents();
        SDL_RenderPresent(renderer);
        sleep(STEP_DELAY / 1000);
      }

    }


    return 0;
}

