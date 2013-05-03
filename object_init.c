#include "object_init.h"

void load_respawn(object *o, int n)
{
    switch(n) {
        case APPLE:
            o->respawn_interval = APPLE_RESPAWN_INTERVAL;
            break;
        case PROJECTILE:
            o->respawn_interval = (int) PROJECTILE_RESPAWN_INTERVAL * rand_between(1.0, 2.0);
            break;
        case BIRD:
            o->respawn_interval = (int) BIRD_RESPAWN_INTERVAL * rand_between(1.0, 2.0);
            break;
    }
}

void reset_objects(object *objects)
{
    enum object_ctr i;
    for (i = 0; i < LAST_MOVER; i++) {
        objects[i].destroyed = 1;
        objects[i].timer = 0;
        reset_object_physics(objects, i);
    }
    // initially just the ground, newton and apple visible on screen
    objects[GROUND].destroyed = 0;
    objects[APPLE].destroyed = 0;
    objects[NEWTON].destroyed = 0;
}

void reset_object_position(object *objects, int object_n)
{
    int newton_width, newton_height;
    object *o = &objects[object_n];
    o->x_pos = o->y_pos = 0;

    switch (object_n) {
        case APPLE:
            o->x_pos = INIT_APPLE_X; o->y_pos = INIT_APPLE_Y;
            break;
        case PROJECTILE:
            newton_width = al_get_bitmap_height(objects[NEWTON].sprite1);
            newton_height = al_get_bitmap_width(objects[NEWTON].sprite1);
            o->x_pos = objects[NEWTON].x_pos + newton_width + 5;
            o->y_pos = objects[NEWTON].y_pos - newton_height - 5;
            break;
        case BIRD:
            o->x_pos = -al_get_bitmap_width(objects[NEWTON].sprite1);
            o->y_pos = rand_between(0, CANVAS_HEIGHT/2);
            break;
        case NEWTON:
            o->x_pos = CANVAS_HEIGHT/10;
            int newton_height = al_get_bitmap_height(objects[NEWTON].sprite1);
            objects[NEWTON].y_pos = objects[GROUND].y_pos - newton_height - 1;
            break;
    }
}

extern const float G;   // declared in physics.c
void reset_object_physics(object *objects, int object_n)
{
    reset_object_position(objects, object_n);

    object *o = &objects[object_n];
    o->x_vel = o->y_vel = o->x_acc = o->y_acc = 0;

    switch (object_n) {
        case APPLE:
            o->x_vel = o->reset_x_vel;
            o->x_acc = 0.01;
            break;

        case PROJECTILE:
            o->y_vel = -3;
            // must go to right: +ve y velocity
            o->x_vel = rand_between(MIN_PROJ_VEL, MAX_PROJ_VEL);
            // must go up: -ve y velocity
            o->y_vel = -rand_between(MIN_PROJ_VEL, MAX_PROJ_VEL);
            o->x_acc = 0; o->y_acc = G;
            break;

        case BIRD:
            o->x_vel = 1;
            break;

        case NEWTON:
            break;      // Newton stays still, nothing to do :)
    }
}