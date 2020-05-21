#ifndef PARTICLE
#define PARTICLE
#include "particle.h"
#endif

#define NUM_PARTICLES 5
#define BALL_RADIUS 10
#define GRAVITY 9.8

// If a particle falls below this velocity it is considered grounded
#define GROUNDED_THRESHOLD 0.5

// If a particle rises above this velocity it is considered ungrounded
#define GROUNDED_EXIT_THRESHOLD 3

// Coefficient of restitution (energy retained after collision)
#define COEFF_REST 0.8

// Coefficient of friction
#define COEFF_FRIC 0.8

Particle particles[NUM_PARTICLES];

// Initialise the graphics engine
void initPhysics(int _minX, int _minY, int _maxX, int _maxY);

// Apply gravity to a particle over a given time `dt`
void applyGravity(Particle *particle, float dt);

// Apply the container's force to a particle
void applyContainerForce(Particle *particle);

// Apply collisions between particles
void applyCollisions();

// Apply an arbitrary force to a particle
void applyForce(Particle *particle, Vector2 force, float dt);

