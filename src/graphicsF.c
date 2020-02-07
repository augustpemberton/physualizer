#include "graphicsF.h"
#include "fortuna.h"

float deltaTime;
float timescalee = 1;

int userQuit = 0;

#define STEP_DELAY 15

const int STEP_TIMESCALE = 0.3;

void initGraphics(int width, int height) {
  initFortuna();
}

double getDT() {
  timescalee -= enc_delta() * STEP_TIMESCALE;
  deltaTime = (getTimerDelta()/100000) * timescalee;
  return deltaTime;
}

void clearParticles(Particle particles[], int n) {
  for (int i=0; i < n; ++i) {
    Particle *particle = &particles[i];
    drawCircle( particle->position.x, particle->position.y, particle->radius, BLACK);
  }
}

void drawParticles(Particle particles[], int n) {
  for (int i=0; i < n; ++i) {
    Particle *particle = &particles[i];
    drawCircle( particle->position.x, particle->position.y, particle->radius, WHITE);
  }
}

void drawBounds(int MIN_X, int MIN_Y, int MAX_X, int MAX_Y) {
  drawLine(MIN_X, MIN_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MIN_X, MIN_Y, MIN_X, MAX_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MIN_X, MAX_Y, GRAY);
}

void waitForNextFrame() {
  _delay_ms(STEP_DELAY);
}