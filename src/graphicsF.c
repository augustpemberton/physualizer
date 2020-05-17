#include "graphicsF.h"
#include "fortuna.h"

float deltaTime;
float timescalee = 1;

int userQuit = 0;

#define STEP_DELAY 15

const int STEP_TIMESCALE = 0.3;

void initGraphics() {
  initFortuna();
}

double getDT() {
  timescalee -= enc_delta() * STEP_TIMESCALE;
  deltaTime = (getTimer()/100000) * timescalee;
  return deltaTime;
}

void resetDT() {
  resetTimer();
}

void clearParticle(Particle particle) {
  drawCircle( particle.position.x, particle.position.y, particle.radius, BLACK);
}

void drawParticle(Particle particle, uint16_t color) {
  drawCircle( particle.position.x, particle.position.y, particle.radius, color);
}

void redrawParticles(Particle oldParticles[], Particle particles[], int n) {
  for (int i=0; i < n; ++i) {
    if (oldParticles[i].position.x != particles[i].position.x &&
        oldParticles[i].position.y != particles[i].position.y) {
      cli();
      clearParticle(oldParticles[i]);
      drawParticle(particles[i], WHITE);
      sei();
    }
  }
}

void drawBounds(int MIN_X, int MIN_Y, int MAX_X, int MAX_Y) {
  drawLine(MIN_X, MIN_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MIN_X, MIN_Y, MIN_X, MAX_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MIN_X, MAX_Y, GRAY);
}

void waitForNextFrame() {
  resetDT();
  _delay_ms(STEP_DELAY);
}