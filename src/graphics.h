#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

extern int userQuit;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Surface* surface;

void initWindow();
void drawFrame();
void handleEvents();

void drawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius);