#include <stdbool.h>

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  float radius;
  float mass;
  bool grounded;
} Particle;