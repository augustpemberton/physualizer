#include "graphicsF.h"
#include "fortuna.h"

int userQuit = 0;

#define STEP_DELAY 15

void initGraphics() {
  initFortuna();
}

double getDT() {
  return getTimer()/100000;
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
    cli();
    clearParticle(oldParticles[i]);
    drawParticle(particles[i], WHITE);
    sei();
  }
}

void drawForce(int force) {
  char str[80];
  display_string_xy(str, 10, 10);
  sprintf(str, "Force: %i   ", force);
  display_string_xy(str, 10, 10);
}

void drawBounds(int MIN_X, int MIN_Y, int MAX_X, int MAX_Y) {
  drawLine(MIN_X, MIN_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MIN_X, MIN_Y, MIN_X, MAX_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MIN_X, MAX_Y, GRAY);
}

void waitForNextFrame() {
  double t = 0;
  t = getTimer();
  while(t < 50000) {
    t = getTimer();
  }
  resetDT();
}