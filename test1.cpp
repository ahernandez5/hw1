
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 4000
#define GRAVITY 0.1


struct Vec {
float x, y, z;

};

struct Shape {
float width, height;
float radius;
Vec center;
};
struct Particle {
    Shape s;
    Vec velocity;
};

class Game {
    public:
    Shape box;
    Particle particle[MAX_PARTICLES];
    int n;
    Game(){
        n = 0;
    }
};



#define rnd () (float) rand() / (float) RAND_MAX

void makeParticle(Game *game, int x, int y) {
    if (game->n >= MAX_PARTICLES)
        return;
   std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    Particle *p = &game->particle[game->n];
    p->s.center.x = x;
    p->s.center.y = y;
   // p->velocity.y = rnd() * 1.0 - 0.5;
  //  p->velocity.x = rnd() * 1.0 - 0.5;
     //  setVelocity(p);
        game->n++;
};

