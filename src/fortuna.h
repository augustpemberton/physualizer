#include <stdlib.h>

// Get the time passed since the last getTimerDelta call.
// Synactic sugar for calling getTimer and then resetTimer.
float getTimerDelta();

// Get the value of the physics engine timer
float getTimer();

// Reset the value of the physics engine timer
void resetTimer();

// Initialise the fortuna
void initFortuna();