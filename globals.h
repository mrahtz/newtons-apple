#ifndef GLOBALS_H
#define GLOBALS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

enum object_ctr
{
    APPLE,
    PROJECTILE,
    BIRD,
    NEWTON,
    LAST_OBJECT
};

typedef struct
{
    int sprite_n;
    ALLEGRO_BITMAP *sprite1;
    ALLEGRO_BITMAP *sprite2;
    float x_acc, y_acc; // acceleration
    float x_vel, y_vel; // velocity
    float x_pos, y_pos; // position
    int destroyed;      // whether the object should be drawn
    int timer;          // time since object destroyed (0 = don't count)
} object;

extern const float G;
extern const int CANVAS_WIDTH;
extern const int CANVAS_HEIGHT;
extern const int PROJ_INTERVAL;
extern const int MIN_PROJ_VEL;
extern const int MAX_PROJ_VEL;
extern const int ANIMATE_TIME;

#endif
