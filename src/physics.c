#include <stdbool.h>
#include <stdlib.h>
#include "physics.h"

int minX = 0;
int minY = 0;
int maxX = 0;
int maxY = 0;

int randRange(int min, int max) {
  return (rand() % (max + 1 - min) + min);
}

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
    scale = (rand() / (double)RAND_MAX) * 3;
    particles[i].radius   = BALL_RADIUS * scale;

    particles[i].position = (Vector2){minX + ((maxX-minX) / NUM_PARTICLES) * i + particles[i].radius,
                                      minY + ((maxY-minY) / NUM_PARTICLES) * i + particles[i].radius};
    particles[i].velocity = (Vector2){10, 10};

    particles[i].mass     = scale;
    particles[i].grounded = false;
  }
}

/*void initializeParticles() {
  for (int i=0; i<NUM_PARTICLES; ++i) {
    particles[i].radius   = (1 + i) * 10;
    particles[i].mass     = 1 + i;
    particles[i].grounded = false;
  }
  //bottom left
  //particles[0].position = (Vector2){50, 200};
  //particles[0].velocity = (Vector2){50, -50};

  //bottom right
  //particles[0].position = (Vector2){200, 200};
  //particles[0].velocity = (Vector2){-50, -50};

  //top right
  particles[1].position = (Vector2){200, 50};
  particles[1].velocity = (Vector2){-50, 50};

  //top left
  particles[0].position = (Vector2){50, 50};
  particles[0].velocity = (Vector2){50, 50};
}*/

Vector2 gravityForce(Particle *particle) {
  return (Vector2){0, particle->mass * GRAVITY};
}

void applyForce(Particle *particle, Vector2 force, float dt) {
  Vector2 acceleration = (Vector2){force.x / particle->mass, force.y / particle->mass};
  particle->velocity.x += acceleration.x * dt;
  particle->velocity.y += acceleration.y * dt;
}

void applyGravity(Particle *particle, float dt) {
  checkUngrounded(particle);

  if (!particle->grounded) {
    applyForce(particle, gravityForce(particle), dt);
  }
}

void applyContainerForce(Particle *particle) {
  if (particle->position.x - particle->radius < minX) {
    particle->velocity.x = -particle->velocity.x * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x += particle->radius - (particle->position.x - minX );
  }
  if (particle->position.x + particle->radius > maxX) {
    particle->velocity.x = -particle->velocity.x * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x -= particle->radius - (maxX - particle->position.x);
  }
  if (particle->position.y - particle->radius < minY) {
    particle->velocity.y = -particle->velocity.y * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y += particle->radius - (particle->position.y - minY );
  }
  if (particle->position.y + particle->radius > maxY) {
    particle->velocity.y = -particle->velocity.y * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y -= particle->radius - (maxY - particle->position.y);

    checkGrounded(particle);

  }
}

void applyCollisions() {
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

        if (particle->position.x < particle2->position.x) {
          if (particle->position.y > particle2->position.y) {
            //printf("Case 1");
            particle->position = (Vector2){particle->position.x-dtx, particle->position.y-dty};
            particle2->position = (Vector2){particle2->position.x+dtx, particle2->position.y+dty};
          } else {
            //printf("Case 2");
            particle->position = (Vector2){particle->position.x-dtx, particle->position.y-dty};
            particle2->position = (Vector2){particle2->position.x+dtx, particle2->position.y+dty};

          }
        } else {
          if (particle->position.y > particle2->position.y) {
            //printf("Case 3");
            particle->position = (Vector2){particle->position.x+dtx, particle->position.y+dty};
            particle2->position = (Vector2){particle2->position.x-dtx, particle2->position.y-dty};
          } else {
            //printf("Case 4");
            particle->position = (Vector2){particle->position.x+dtx, particle->position.y+dty};
            particle2->position = (Vector2){particle2->position.x-dtx, particle2->position.y-dty};
          }
        }


        // A.v = (A.u * (A.m - B.m) + (2 * B.m * B.u)) / (A.m + B.m)
        // B.v = (B.u * (B.m - A.m) + (2 * A.m * A.u)) / (A.m + B.m)

        Vector2 au = particle->velocity;
        Vector2 bu = particle2->velocity;
        float k = particle->mass + particle2->mass;

        particle->velocity.x = (au.x * (particle->mass - particle2->mass) + (2 * particle2->mass * bu.x)) / k;
        particle->velocity.y = (au.y * (particle->mass - particle2->mass) + (2 * particle2->mass * bu.y)) / k;

        particle2->velocity.x = (bu.x * (particle2->mass - particle->mass) + (2 * particle->mass * au.x)) / k;
        particle2->velocity.y = (bu.y * (particle2->mass - particle->mass) + (2 * particle->mass * au.y)) / k;

        //checkGrounded(particle);

      }
    }
  }
}

void checkGrounded(Particle *particle) {
  float speed = sqrt(pow(particle->velocity.x, 2) + pow(particle->velocity.y, 2));
  if (speed < GROUNDED_THRESHOLD) {
    particle->grounded = true;
    particle->velocity = (Vector2){0, 0};
  }
}

void checkUngrounded(Particle *particle) {
  if (abs(particle->velocity.x) > 0.0001 || abs(particle->velocity.y) > 0.0001) {
    particle->grounded = false;
  }
}