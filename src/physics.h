#define NUM_PARTICLES 5
#define BALL_RADIUS 10
#define GRAVITY 9.8
#define GROUNDED_THRESHOLD 0.4
#define GROUNDED_EXIT_THRESHOLD 0.5

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

void applyGravity(Particle *particle, float dt);
void applyContainerForce(Particle *particle);
void applyCollisions();
