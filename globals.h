#ifndef GLOBALS_H
#define GLOBALS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

enum game_scenes
{
    TITLE,
    INTRO,
    GAME,
    GAMEOVER
};

enum
{
    CANVAS_WIDTH = 640,
    CANVAS_HEIGHT = 480,
    MIN_PROJ_VEL = 5,
    MAX_PROJ_VEL = 10,
    ANIMATE_TIME = 30,
    INIT_SPAWN_INTERVAL = 120,
    APPLE_RESPAWN_INTERVAL = 30,
    PROJECTILE_RESPAWN_INTERVAL = 120,
    BIRD_RESPAWN_INTERVAL = 110
};

enum object_ctr
{
    APPLE,
    PROJECTILE,
    BIRD,
    NEWTON,
    LAST_OBJECT,
    // after LAST_OBJECT so it's not accessed during the normal 
    // i < LAST_OBJECT iterations
    GROUND
};

typedef struct
{
    ALLEGRO_BITMAP *sprite1;
    ALLEGRO_BITMAP *sprite2;
    float x_acc, y_acc; // acceleration
    float x_vel, y_vel; // velocity
    float x_pos, y_pos; // position
    int destroyed;      // whether the object should be drawn
    int timer;          // time since object destroyed (0 = don't count)
    int respawn_interval;
} object;

#endif
