#include <time.h>
#include "graphics.h"

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Renderer* renderer = NULL;

int userQuit = 0;

clock_t previousTime;
clock_t currentTime;
float deltaTime;

#define STEP_DELAY 15

bool buttonPressed = false;

void initGraphics(int width, int height) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialise SDL: %s\n", SDL_GetError());
    return;
  }

  if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) != 0){
    printf("Could not create window / renderer: %s\n", SDL_GetError());
    return;
  }

  previousTime = clock();
  currentTime = clock();

}

double getDT() {
  currentTime = clock();
  deltaTime = (double) (currentTime - previousTime);
  previousTime = currentTime;
  return deltaTime * 0.000018;
}

void handleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        SDL_DestroyWindow(window);
        SDL_Quit();
        userQuit = 1;
        break;
      
      case SDL_MOUSEBUTTONDOWN:
        buttonPressed = true;
        break;

      case SDL_MOUSEBUTTONUP:
        buttonPressed = false;
        break;
    }
  }
}

void drawParticle(int centreX, int centreY, int radius)
{
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void drawBounds(int MIN_X, int MIN_Y, int MAX_X, int MAX_Y) {
  SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MAX_X, MIN_Y);
  SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MIN_X, MAX_Y);
  SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MAX_X, MIN_Y);
  SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MIN_X, MAX_Y);
}

void clearParticles(Particle particles[], int n) {
  SDL_RenderClear(renderer);
}

void drawParticles(Particle particles[], int n) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  for (int i=0; i < n; ++i) {
    Particle *particle = &particles[i];
    if (particle->grounded) {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    drawParticle(particle->position.x, particle->position.y, particle->radius);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void waitForNextFrame() {
  handleEvents();
  SDL_RenderPresent(renderer);
  sleep(STEP_DELAY/1000);
}