#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "src/graphics.h"

void init(void);
void handleEvents();
void drawFrame();

int8_t enc_delta(void);
volatile int8_t delta;

#define TIMESCALE_STEP 0.2
float timescale = 1;

#define NUM_PARTICLES 6

#define STEP_DELAY 0
#define BALL_RADIUS 10
#define GRAVITY 9.8
#define GROUNDED_THRESHOLD 0.4
#define GROUNDED_EXIT_THRESHOLD 0.5

#define MIN_X 0
#define MIN_Y 0
#define MAX_X 320
#define MAX_Y 240

#define COEFF_REST 0.8
#define COEFF_FRIC 0.8

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  float radius;
  float mass;
  float restitution;
  float friction;
  bool grounded;
} Particle;

Particle particles[NUM_PARTICLES];

int randRange(int min, int max) {
  return (rand() % (max + 1 - min) + min);
}

void drawBounds() {
  SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MAX_X, MIN_Y);
  SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MIN_X, MAX_Y);
  SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MAX_X, MIN_Y);
  SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MIN_X, MAX_Y);
}

void drawParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    if (particle->grounded) {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
    drawCircle(renderer, particle->position.x, particle->position.y, particle->radius);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void clearParticles() {
  SDL_RenderClear(renderer);
}

void initializeParticles() {
  float scale;
  for (int i=0; i<NUM_PARTICLES; ++i) {
    scale = 1;
    particles[i].radius   = BALL_RADIUS * scale;

    particles[i].position = (Vector2){MIN_X + ((MAX_X-MIN_X) / NUM_PARTICLES) * i,
                                      MIN_Y + ((MAX_Y-MIN_Y) / NUM_PARTICLES) * i};
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

void applyContainerForce(Particle *particle) {
  if (particle->position.x - particle->radius < MIN_X) {
    particle->velocity.x = abs(particle->velocity.x) * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x += particle->radius - (particle->position.x - MIN_X );
  }
  if (particle->position.x + particle->radius > MAX_X) {
    particle->velocity.x = -1 * abs(particle->velocity.x) * COEFF_REST;
    particle->velocity.y *= COEFF_FRIC;

    particle->position.x -= particle->radius - (MAX_X - particle->position.x);
  }
  if (particle->position.y - particle->radius < MIN_Y) {
    particle->velocity.y = abs(particle->velocity.y) * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y += particle->radius - (particle->position.y - MIN_Y );
  }
  if (particle->position.y + particle->radius > MAX_Y) {
    particle->velocity.y = -1 * abs(particle->velocity.y) * COEFF_REST;
    particle->velocity.x *= COEFF_FRIC;

    particle->position.y -= particle->radius - (MAX_Y - particle->position.y);

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

clock_t previousTime;
clock_t currentTime;
float deltaTime;
double getDT() {
  currentTime = clock();
  deltaTime = (double) (currentTime - previousTime);
  previousTime = currentTime;
  return deltaTime;
  //deltaTime = TCNT1;
  //dt = (float) deltaTime * timescale * 0.00001;
  //TCNT1 = 0;
}

int main(void) {
    init();
    float dt = 0;
    while (userQuit == 0) {
      drawBounds();
      handleEvents();
      dt = (getDT() / 100000) * timescale;

      clearParticles();
      applyCollisions();
      for (int i=0; i<NUM_PARTICLES; ++i) {
        Particle *particle = &particles[i];

        if (particle->grounded == true && abs(particle->velocity.y) > GROUNDED_EXIT_THRESHOLD) {
          particle->grounded = false;
        }

        if (particle->grounded) {
          particle->velocity.y = abs(particle->velocity.y);
        }

        if (particle->grounded == false) {
          applyForce(particle, gravityForce(particle), dt);
        }

        applyContainerForce(particle);
        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }

      drawParticles();

      SDL_RenderPresent(renderer);

      //_delay_ms(STEP_DELAY);
      sleep(STEP_DELAY / 1000);
    }
    return 0;
}

void init(void) {
  initWindow();
  previousTime = clock();
  currentTime = clock();
  //srand(time(NULL));
  initializeParticles();
}