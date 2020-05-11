#ifndef PARTICLE
#define PARTICLE
#include "particle.h"
#endif

#include <SDL2/SDL.h>

extern int userQuit;
extern bool buttonPressed;
extern bool speedUpPressed;
extern bool slowDownPressed;

void initGraphics(int width, int height);
void handleEvents();

void clearParticles(Particle particles[], int n);
void drawParticles(Particle particles[], int n);

void waitForNextFrame();

double getDT();
