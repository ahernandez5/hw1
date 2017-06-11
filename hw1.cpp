//modified by: Aurora Hernandez
//date: 06/11/17
//purpose: Git
//
//cs3350 Spring 2017 Lab-1
//author: Gordon Griesel
//date: 2014 to present
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h" //include fonts.h

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 4000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

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
    Shape box[5]; // specified to 5 boxes 
    Shape circle; // for circle
    Particle particle[MAX_PARTICLES];
    int n;

    Game()
    {
        n = 0;
    }
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);
void displayFont (void);//

int main(void)
{
    int done = 0;
    srand(time(NULL));
    initXWindows();
    init_opengl();
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
    //declare game object
    Game game;
    //  game.n=0;
    

    //declare a box shape
    for (int i = 0; i < 5; i++) { // loop from 0 to 5 boxes

        game.box[i].width = 200;
        game.box[i].height = 20;
        game.box[i].center.x = 225 + i * 70;
        game.box[i].center.y = 525 - i * 70;
    }
    game.circle.center.y = 0;
    game.circle.center.x = WINDOW_WIDTH;// circle y */
    game.circle.radius = 100;
    //start animation
    while (!done) {
        while (XPending(dpy)) {
            XEvent e;
            XNextEvent(dpy, &e);
            check_mouse(&e, &game);
            done = check_keys(&e, &game);
        }
        movement(&game);
        render(&game);
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    //CLEANUP FONTS 
    cleanup_fonts();
    return 0;
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
    //do not change
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void initXWindows(void)
{
    //do not change
    GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    int w = WINDOW_WIDTH, h = WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        std::cout << "\n\tcannot connect to X server\n" << std::endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
        std::cout << "\n\tno appropriate visual found\n" << std::endl;
        exit(EXIT_FAILURE);
    }
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPress | ButtonReleaseMask | PointerMotionMask |
            StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
            InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

#define rnd() (float)rand() / (float)RAND_MAX

void makeParticle(Game *game, int x, int y)
{
    if (game->n >= MAX_PARTICLES)
        return;
    //std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    Particle *p = &game->particle[game->n];
    p->s.center.x = x;
    p->s.center.y = y;
    p->velocity.y = rnd() * 1.0 - 0.5;
    p->velocity.x = rnd() * 1.0 - 0.5;
    game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
    static int savex = 0;
    static int savey = 0;
    static int n = 0;

    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button == 1) {
            //Left button was pressed
            int y = WINDOW_HEIGHT - e->xbutton.y;
            makeParticle(game, e->xbutton.x, y);
            makeParticle(game, e->xbutton.x, y);
            makeParticle(game, e->xbutton.x, y);
            makeParticle(game, e->xbutton.x, y);
            makeParticle(game, e->xbutton.x, y);
            return;
        }
        if (e->xbutton.button == 3) {
            //Right button was pressed
            return;
        }
    }
    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x;
        savey = e->xbutton.y;
        if (++n < 10)
            return;
        int y = WINDOW_HEIGHT - e->xbutton.y;
        for (int i = 0; i < 10; i++)
            makeParticle(game, e->xbutton.x, y);

    }
}

int check_keys(XEvent *e, Game *game)
{
    //Was there input from the keyboard?
    if (e->type == KeyPress) {
        int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_Escape) {
            return 1;
        }
        //You may check other keys here.



    }
    return 0;
}

void movement(Game *game)
{
    Particle *p;

    if (game->n <= 0)
        return;
//    if (makeParticle(game, e->xbutton.x, y) {
//        
//        
//        
//    }; 
    for (int i = 0; i < game->n; i++) {
        p = &game->particle[i];
        p->velocity.y -= GRAVITY;
        p->s.center.x += p->velocity.x;
        p->s.center.y += p->velocity.y;

        //check for collision with shapes...

        for (int index = 0; index < 5; index++) {
            Shape *s = &game->box[index]; //**
            if (    p->s.center.y < s->center.y + s->height &&
                    p->s.center.x > s->center.x - s->width &&
                    p->s.center.x < s->center.x + s->width) {
                p->s.center.y = s->center.y + s->height;
                p->velocity.y = -p->velocity.y;
                p->velocity.y *= 0.5;

            }
        }
        //check for off-screen
        if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
            //std::cout << "off screen" << std::endl;
            game->particle[i] = game->particle[game->n - 1];
            game->n--;
        }
    }
}

const char* titles[] = { // constant character with array to display text
    "Requirements",
    "Design",
    "Coding",
    "Testing",
    "Maintenance"
};


void render(Game *game)
{
    float w, h;
    glClear(GL_COLOR_BUFFER_BIT);
    //Draw shapes...

    //draw box
    Shape *s;
    for (int i = 0; i < 5; i++) { // ***      
        //glColor3ub(90,140,90);
        glColor3ub(110, 50, 120);
        s = &game->box[i];
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
        glBegin(GL_QUADS);
        glVertex2i(-w, -h);
        glVertex2i(-w, h);
        glVertex2i(w, h);
        glVertex2i(w, -h);
        glEnd();
        
        //glEnable(GL_TEXTURE_2D);
        //draw text
        Rect r; 
        unsigned int c = 0x00ffff44; // Displays text on Waterfall Model
        //r.bot = y - 20;
        r.left = 10;
        r.center = 10;
        ggprint12(&r, 1, c, titles[i]);
        
        
        
        glPopMatrix();
    }
    //draw all particles here
    glPushMatrix();
    glColor3ub(70, 160, 220);
    for (int i = 0; i < game->n; i++) {
        Vec *c = &game->particle[i].s.center;
        w = 2;
        h = 2;
        glBegin(GL_QUADS);
        glVertex2i(c->x - w, c->y - h);
        glVertex2i(c->x - w, c->y + h);
        glVertex2i(c->x + w, c->y + h);
        glVertex2i(c->x + w, c->y - h);
        glEnd(); 
        glPopMatrix();
    }
}
    //glPushMatrix();

   
//    int num = 360;
//    glColor3f(0.0,0.8,0.8);
//    GLfloat r = game->circle.radius;// circle radius used to daw circle    
//    GLfloat pix2 = 2.0f * 3.141592;
//    GLint circle_x = game->circle.center.x;
//    GLuint circle_y = game->circle.center.y;
//
//    glBegin(GL_TRIANGLE_FAN);
//        glVertex2i(circle_x, circle_y); // center of circle
//        for (int i = 0; i <= num; i++) {
//            glVertex2i(
//                circle_x + (r * cos(i * pix2 / num)),
//               circle_y + (r * sin(i * pix2 / num))
//            );
       //         glEnd();
    //glPopMatrix();
//}
//    glEnd();
//    glPopMatrix();
//}

    
    
      
        
       
    

    

