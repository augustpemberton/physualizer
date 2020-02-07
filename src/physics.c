#include <stdbool.h>
#include "physics.h"

int minX;
int minY;
int maxX;
int maxY;

void initPhysics(int _minX, int _minY, int _maxX, int _maxY) {
  minX = _minX;
  maxX = _maxX;
  minY = _minY;
  maxY = _maxY;
  initializeParticles();
}

void initializeParticles() {
  float scale;
  for (int i=0; i<NUM_PARTICLES; ++i) {
    scale = 1;
    particles[i].radius   = BALL_RADIUS * scale;

    particles[i].position = (Vector2){minX + ((maxX-minX) / NUM_PARTICLES) * i,
                                      minY + ((maxY-minY) / NUM_PARTICLES) * i};
    particles[i].velocity = (Vector2){randRange(-50, 50), randRange(-50, 50)};

    particles[i].mass     = scale;
    particles[i].grounded = false;
  }
}

Vector2 gravityForce(Particle *particle) {
  return (Vector2){0, particle->mass * GRAVITY};
}

void applyForce(Particle *particle, Vector2 force, float dt) {
  Vector2 acceleration = (Vector2){force.x / particle->mass, force.y / particle->mass};
  particle->velocity.x += acceleration.x * dt;
  particle->velocity.y += acceleration.y * dt;
}

void applyGravity(Particle *particle, float dt) {
  if (particle->grounded == true && abs(particle->velocity.y) > GROUNDED_EXIT_THRESHOLD) {
    particle->grounded = false;
  }

  if (!particle->grounded) {
    applyForce(particle, gravityForce(particle), dt);
  } else {
    particle->velocity.y = abs(particle->velocity.y);
  }
}

void applyContainerForce(Particle *particle) {
  if (particle->position.x - particle->radius < minX) {
    particle->velocity.x = abs(particle->velocity.x) * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x += particle->radius - (particle->position.x - minX );
  }
  if (particle->position.x + particle->radius > maxX) {
    particle->velocity.x = -1 * abs(particle->velocity.x) * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x -= particle->radius - (maxX - particle->position.x);
  }
  if (particle->position.y - particle->radius < minY) {
    particle->velocity.y = abs(particle->velocity.y) * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y += particle->radius - (particle->position.y - minY );
  }
  if (particle->position.y + particle->radius > maxY) {
    particle->velocity.y = -1 * abs(particle->velocity.y) * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y -= particle->radius - (maxY - particle->position.y);

    if (abs(particle->velocity.y) < GROUNDED_THRESHOLD && abs(particle->velocity.x) < GROUNDED_THRESHOLD) {
      particle->grounded = true;
    }
  }
}

void applyCollisions() {
  // iterate through each particle
  // check its not our particle
  // apply collision forces
  float distance;
  for (int j=0; j<NUM_PARTICLES; ++j) {
    Particle *particle = &particles[j];
    for (int i=j+1; i<NUM_PARTICLES; ++i) {
      Particle *particle2 = &particles[i];
      distance = sqrt(pow(particle->position.x - particle2->position.x, 2) + 
                      pow(particle->position.y - particle2->position.y, 2));
      if (distance < (particle->radius + particle2->radius)) {
        float dx = particle->position.x - particle2->position.x;
        float dy = particle->position.y - particle2->position.y;

        float distanceToMove = (particle->radius + particle2->radius) - sqrt(pow(dx,2) + pow(dy,2));

        float angle = atan(dy/dx);

        float dtx = distanceToMove * cos(angle);
        float dty = distanceToMove * sin(angle);

        particle->position = (Vector2){particle->position.x-abs(dtx), particle->position.y-abs(dty)};
        particle2->position = (Vector2){particle2->position.x+abs(dtx), particle2->position.y+abs(dty)};

        // A.v = (A.u * (A.m - B.m) + (2 * B.m * B.u)) / (A.m + B.m)
        // B.v = (B.u * (B.m - A.m) + (2 * A.m * A.u)) / (A.m + B.m)

        Vector2 au = particle->velocity;
        Vector2 bu = particle2->velocity;
        float k = particle->mass + particle2->mass;

        particle->velocity.x = (au.x * (particle->mass - particle2->mass) + (2 * particle2->mass * bu.x)) / k;
        particle->velocity.y = (au.y * (particle->mass - particle2->mass) + (2 * particle2->mass * bu.y)) / k;

        particle2->velocity.x = (bu.x * (particle2->mass - particle->mass) + (2 * particle->mass * au.x)) / k;
        particle2->velocity.y = (bu.y * (particle2->mass - particle->mass) + (2 * particle->mass * au.y)) / k;

        // check for grounded state
        if (particle->position.y < particle2->position.y) {
          if (abs(particle->velocity.y) < GROUNDED_THRESHOLD && abs(particle->velocity.x) < GROUNDED_THRESHOLD) {
            particle->grounded = true;
          }
        } else {
          if (abs(particle2->velocity.y) < GROUNDED_THRESHOLD && abs(particle2->velocity.x) < GROUNDED_THRESHOLD) {
            particle2->grounded = true;
          }
        }

      }
    }
  }
}