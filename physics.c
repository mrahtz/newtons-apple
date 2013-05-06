#include "physics.h"

const float G = 0.1;

// returns 1 if apple was hit, otherwise 0
int simulate_objects(object *objects, float audio_level)
{
    enum object_ctr i;

    if (objects[APPLE].invincibility_timer != 0)
        objects[APPLE].invincibility_timer--;

    for (i = 0; i < LAST_MOVER; i++) {
        object *o = &objects[i];

        // handle respawn timing
        if  (o->timer != 0)
            o->timer--;
        else if (o->destroyed && o->timer == 0) {   // time to respawn
            o->timer = 0;
            o->destroyed = 0;
            if (i == APPLE)
                o->invincibility_timer = APPLE_INVINCIBILITY_TIME;
            reset_object_physics(objects, i);
        }
        if (o->destroyed)
            continue;

        if (i == APPLE)
            o->y_acc = G - MIC_SENSITIVITY*audio_level;

        update_physics(objects, i, MODE_WRT_APPLE);
        //printf("%f %f\n", o->x_pos, o->y_pos);

        if (i != APPLE &&
                !objects[APPLE].destroyed &&
                objects[APPLE].invincibility_timer == 0 &&
                check_collision(o, &objects[APPLE]) == 1) {
            objects[APPLE].destroyed = 1;
            objects[APPLE].timer = objects[APPLE].respawn_interval;
        }

        if (i == APPLE && o->y_pos < 0) {
            o->y_pos = 0; // cap the apple to the top of the screen
            o->y_vel = 0;
        }
        if (!o->destroyed &&
            (check_if_offscreen(o) == 1 ||
             check_ground_collision(o, &objects[GROUND]) == 1) ) {
            o->destroyed = 1;
            o->timer = o->respawn_interval;
        }
    }
    
    // apple has just been destroyed
    if (objects[APPLE].timer == objects[APPLE].respawn_interval)
        return 1;
    else
        return 0;
}

int check_collision(object *o1, object *o2)
{
    // half-dimensions of object 1
    int o1_hwidth = al_get_bitmap_width(o1->sprite1)/2;
    int o1_hheight = al_get_bitmap_height(o1->sprite1)/2;
    // half-dimensions of object 2
    int o2_hwidth = al_get_bitmap_width(o2->sprite1)/2;
    int o2_hheight = al_get_bitmap_height(o2->sprite1)/2;

    // centre of object 1
    int o1_centre_x = o1->x_pos + o1_hwidth;
    int o1_centre_y = o1->y_pos + o1_hheight;
    // centre of object 2
    int o2_centre_x = o2->x_pos + o2_hwidth;
    int o2_centre_y = o2->y_pos + o2_hheight;

    // if the distance between the centres is less than
    // the sum of the half-distances in both dimensions,
    // they're colliding
    if ( abs(o2_centre_x - o1_centre_x) < (o2_hwidth + o1_hwidth) &&
         abs(o2_centre_y - o1_centre_y) < (o2_hheight + o1_hheight) )
        return 1;
    else
        return 0;
}

int check_ground_collision(object *o, object *ground)
{
    int object_height = al_get_bitmap_height(o->sprite1);
    int object_bottom_y = o->y_pos + object_height - 1;
    int collided = object_bottom_y >= ground->y_pos;

    return collided;
}

void update_physics(object *objects, int n, int mode)
{
    object *o = &objects[n];

    o->x_vel += o->x_acc;
    o->y_vel += o->y_acc;

    if (mode == MODE_ABSOLUTE) {
        o->x_pos += o->x_vel;
    } else if (mode == MODE_WRT_APPLE) {
        if (n == BIRD)
            o->x_pos += o->x_vel + objects[APPLE].x_vel / 5.0;
        if (n != APPLE)
            o->x_pos += o->x_vel;
    }
    o->y_pos += o->y_vel;
}