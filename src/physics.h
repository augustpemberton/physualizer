#ifndef PARTICLE
#define PARTICLE
#include "particle.h"
#endif

#define NUM_PARTICLES 20
#define BALL_RADIUS 10
#define GRAVITY 9.8
#define GROUNDED_THRESHOLD 0.5
#define GROUNDED_EXIT_THRESHOLD 3

#define COEFF_REST 0.8
#define COEFF_FRIC 0.8

Particle particles[NUM_PARTICLES];

void initPhysics(int _minX, int _minY, int _maxX, int _maxY);

void applyGravity(Particle *particle, float dt);
void applyContainerForce(Particle *particle);
void applyCollisions();
void dampenVelocity(Particle *particle, float dt);

void applyForce(Particle *particle, Vector2 force, float dt);

