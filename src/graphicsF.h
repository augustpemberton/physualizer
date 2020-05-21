#ifndef PARTICLE
#define PARTICLE
#include "particle.h"
#endif

#include "unifiedLcd.h"

// Initialise the graphics engine
void initGraphics();

// Clear old particles and draw new ones to the screen.
// Syntactic sugar for using the clearParticles and drawParticles functions
void redrawParticles(Particle oldParticles[], Particle particles[], int n);

// Clear particles from the screen (draw them in black)
void clearParticles(Particle particles[], int n);

// Draw particles to the screen
void drawParticles(Particle particles[], int n);

// Pause execution until the next frame (used if a constant FPS is required)
void waitForNextFrame();

// Get the time passed since the last frame
double getDT();