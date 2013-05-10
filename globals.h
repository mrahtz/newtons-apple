#ifndef GLOBALS_H
#define GLOBALS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

// TODO justify use of enums rather than defines
enum game_states
{
    INIT_TITLE,
    TITLE,
    INIT_INTRO,
    INTRO,
    INSTRUCTIONS,
    INIT_GAME_WITH_RESET,
    INIT_GAME,
    GAME,
    GAMEOVER,
    QUIT
};

enum
{
    CANVAS_WIDTH = 640,
    CANVAS_HEIGHT = 480,
    MIN_PROJ_VEL = 3,
    MAX_PROJ_VEL = 8,
    ANIMATE_TIME = 10,
    INIT_SPAWN_INTERVAL = 120,
    APPLE_RESPAWN_INTERVAL = 30,
    APPLE_INVINCIBILITY_TIME = 360,
    PROJECTILE_RESPAWN_INTERVAL = 60,
    BIRD_RESPAWN_INTERVAL = 110,
    INIT_APPLE_X = CANVAS_WIDTH/2,      // so that the intro sequence can check
    INIT_APPLE_Y = CANVAS_HEIGHT/4,
    MIC_SENSITIVITY = 4
};

enum physics_modes
{
    MODE_ABSOLUTE,
    MODE_WRT_APPLE
};

enum object_ctr
{
    APPLE,
    PROJECTILE,
    BIRD,
    NEWTON,
    LAST_MOVER,
    // after LAST_MOVER so it's not accessed during the normal
    // i < LAST_MOVER iterations
    TREE,
    GROUND,
    OBJECTS_END
};

typedef struct
{
    ALLEGRO_BITMAP *sprite1;
    ALLEGRO_BITMAP *sprite2;
    ALLEGRO_BITMAP *sprite3;    // only used for Newton asleep
    float x_acc, y_acc; // acceleration
    float x_vel, y_vel; // velocity
    float reset_x_vel;    // only used for apple - save vel after instructions finished
    float x_pos, y_pos; // position
    int width, height;
    int destroyed;      // whether the object should be drawn
    int respawn_timer;          // time since object destroyed (0 = don't count)
    int invincibility_timer;    // only used for apple
} object;

typedef struct
{
    ALLEGRO_BITMAP *tree;
    ALLEGRO_BITMAP *instructions1;
    ALLEGRO_BITMAP *instructions2;
    ALLEGRO_FONT *font; // small font for showing labels on Newton
} intro_resource_struct;

typedef struct
{
    int scene;
    int score;
    int lives; 
    int ticks;
} game_state_struct;

#endif
