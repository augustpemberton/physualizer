#include <math.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void init(void);

int8_t enc_delta(void);
volatile int8_t delta;


#define TIMESCALE_STEP 0.2
float timescale = 1;

#define STEP_DELAY 7
#define BALL_RADIUS 10
#define NUM_PARTICLES 1
#define GRAVITY 9.8

#define MIN_X 0
#define MIN_Y 0
#define MAX_X 320
#define MAX_Y 240

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  float radius;
  float mass;
} Particle;

Particle particles[NUM_PARTICLES];

int randRange(int min, int max) {
  return (rand() % (max + 1 - min) + min);
}

float boundX(float n, float p) {
  if (n - p < MIN_X) return MIN_X + p;
  if (n + p > MAX_X) return MAX_X - p;
  return n;
}

float boundY(float n, float p) {
  if (n - p < MIN_Y) return MIN_Y + p;
  if (n + p > MAX_Y) return MAX_Y - p;
  return n;
}

/*
void drawBounds() {
  drawLine(MIN_X, MIN_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MIN_X, MIN_Y, MIN_X, MAX_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MAX_X, MIN_Y, GRAY);
  drawLine(MAX_X, MAX_Y, MIN_X, MAX_Y, GRAY);
}

void drawParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, WHITE);
  }
}

void clearParticles() {
  for (int i=0; i < NUM_PARTICLES; ++i) {
    Particle *particle = &particles[i];
    drawCircle( particle->position.x, particle->position.y, BALL_RADIUS, BLACK);
  }
}
*/

void initializeParticles() {
  for (int i=0; i<NUM_PARTICLES; ++i) {
    particles[i].radius   = BALL_RADIUS;
    particles[i].position = (Vector2){randRange( MIN_X + particles[i].radius, MAX_X - particles[i].radius), 
                                      randRange( MIN_Y + particles[i].radius, MAX_Y - particles[i].radius)};
    particles[i].velocity = (Vector2){40, 30};
    particles[i].mass     = 1;
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
    particle->velocity.x = abs(particle->velocity.x) * 0.8;
    //particle->position.x += 2 * (MIN_X - (particle->position.x - particle->radius));
  }
  if (particle->position.x + particle->radius > MAX_X) {
    particle->velocity.x = -1 * abs(particle->velocity.x) * 0.8;
    //particle->position.x -= 2 * ((particle->position.x + particle->radius) - MAX_X);
  }
  if (particle->position.y - particle->radius < MIN_Y) {
    particle->velocity.y = abs(particle->velocity.y) * 0.8;
    //particle->position.y += 2 * (MIN_Y - (particle->position.y - particle->radius));
  }
  if (particle->position.y + particle->radius > MAX_Y) {
    particle->velocity.y = -1 * abs(particle->velocity.y) * 0.8;
    //particle->position.y -= 2 * ((particle->position.y + particle->radius) - MAX_Y);
  }
}

//double deltaTime;
float getDT() {
  return 0.0001;
  //deltaTime = TCNT1;
  //dt = (float) deltaTime * timescale * 0.00001;
  //TCNT1 = 0;
}

int main(void) {
    init();
    return 0;
    float dt = 0;
    for (;;) {
      dt = getDT();
      //clearParticles();
      for (int i=0; i<NUM_PARTICLES; ++i) {
        Particle *particle = &particles[i];
        applyContainerForce(particle);
        applyForce(particle, gravityForce(particle), dt);
        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
      }
      //drawParticles();
      //_delay_ms(STEP_DELAY);
      sleep(STEP_DELAY / 1000);

    }
}


const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
void initWindow() {
  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialise SDL: %s\n", SDL_GetError());
    return;
  }

  window = SDL_CreateWindow("Physualiser", 
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);

  if (window == NULL) {
    printf("Window could not be created: %s\n", SDL_GetError());
    return;
  }
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ));
  SDL_UpdateWindowSurface(window);
  SDL_Delay(2000);

  SDL_DestroyWindow(window);
  SDL_Quit();
}

void init(void) {
    initializeParticles();
    initWindow();
}