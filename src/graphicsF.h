#ifndef PARTICLE
#define PARTICLE
#include "particle.h"
#endif

#include "unifiedLcd.h"

extern int userQuit;

void initGraphics(int width, int height);
void handleEvents();

void clearParticles(Particle particles[], int n);
void drawParticles(Particle particles[], int n);

void waitForNextFrame();

double getDT();