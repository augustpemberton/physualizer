#ifndef PARTICLE
#define PARTICLE
#include "particle.h"
#endif

#include "unifiedLcd.h"

extern int userQuit;
extern bool buttonPressed;
extern bool speedUpPressed;
extern bool slowDownPressed;

void initGraphics();
void handleEvents();

void redrawParticles(Particle oldParticles[], Particle particles[], int n);

void clearParticles(Particle particles[], int n);
void drawParticles(Particle particles[], int n);

void waitForNextFrame();

double getDT();