#include "globals.h"
#include "game.h"
#include "helpers.h"

void reset_object(object *o, int object_n)
{
    o->x_pos = o->y_pos = o->x_vel = o->y_vel = o->x_acc = o->y_acc = 0;
    o->destroyed = 0; o->timer = 0;

    switch (object_n) {
        case APPLE:
            o->x_pos = CANVAS_WIDTH/2; o->y_pos = CANVAS_HEIGHT/4;
            break;

        case PROJECTILE:
            o->x_pos = 100; o->y_pos = 400;
            o->y_vel = -3;
            // must go to right: +ve y velocity
            o->x_vel = rand_between(MIN_PROJ_VEL, MAX_PROJ_VEL);
            // must go up: -ve y velocity
            o->y_vel = -rand_between(MIN_PROJ_VEL, MAX_PROJ_VEL);
            o->x_acc = 0; o->y_acc = G;
            break;

        case BIRD:
            o->x_pos = CANVAS_WIDTH-1;
            o->y_pos = rand_between(0, CANVAS_HEIGHT/2);
            o->x_vel = -3;
    }
}

int check_if_offscreen(object *o)
{
    int o_width = al_get_bitmap_width(o->sprite1);
    int o_height = al_get_bitmap_height(o->sprite1);

    int offscreen_x = o->x_pos > CANVAS_WIDTH-1 ||
                      o->x_pos - 1 + o_width < 0;
    int offscreen_y = o->y_pos > CANVAS_HEIGHT-1 ||
                      o->y_pos - 1 + o_height < 0;

    return offscreen_x || offscreen_y;
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

void simulate_world(object *objects, float audio_level)
{
    enum object_ctr i;
    for (i = 0; i < LAST_OBJECT; i++) {
        object *o = &objects[i];
        if (i == APPLE)
            o->y_acc = G - audio_level/2;

        o->x_vel += o->x_acc;
        o->y_vel += o->y_acc;

        o->x_pos += o->x_vel;
        o->y_pos += o->y_vel;
        //printf("%f %f\n", o->x_pos, o->y_pos);

        if (i != APPLE && ! objects[APPLE].destroyed && check_collision(o, &objects[APPLE])) {
            printf("CRUSHA\n");
            objects[APPLE].destroyed = 1;
            objects[APPLE].timer = 1;
        }

        if (i == APPLE && o->y_pos < 0) {
            o->y_pos = 0;
            o->y_vel = 0;
        } else if (check_if_offscreen(o) && ! o->destroyed)
        {
            //printf("OVERKILLL\n");
            o->destroyed = 1;
            o->timer = 1;
        }

        if (o->timer != 0)
            o->timer += 1;
        if (o->timer == PROJ_INTERVAL) {
            //printf("RESPAWNNNNN\n");
            o->timer = 0;
            o->destroyed = 0;
            
            reset_object(o, i);
        }

    }
}

/*void rotate_ground(ALLEGRO_BITMAP *ground)
{
    int width = al_get_bitmap_width(ground);
    int height = al_get_bitmap_height(ground);
    al_lock_bitmap
    // set target bitmap

    int y, i;
    for (y = 0; y < height; y++) {
        ALLEGRO_COLOR pix0 = al_get_pixel(ground, 0, y);
        for (i = 0; i < width-2; i++)
            al_put_pixel(i, y, al_get_pixel(ground, i+1, y))
        al_put_pixel(width-1, y, pix0);
    }

    al_unlock_bitmap
}*/

void draw_world(object *objects, ALLEGRO_BITMAP *ground, int animate_timer)
{
    enum object_ctr i;
    int flip_sprites = animate_timer % ANIMATE_TIME == 0;

    al_clear_to_color(al_map_rgb(0, 0, 0));
    for (i = 0; i < LAST_OBJECT; i++) {
        object *o = &objects[i];

        if (flip_sprites)
            o->sprite_n = !(o->sprite_n);
        if (! o->destroyed) {
            if (o->sprite_n == 0) {
                // dx, dy, flags (delta because relative to current postion)
                al_draw_bitmap(o->sprite1, (int) o->x_pos, (int) o->y_pos, 0);
            } else
                al_draw_bitmap(o->sprite2, (int) o->x_pos, (int) o->y_pos, 0);    // dx, dy, flags (delta because relative to curr-> pos->)
        }
    }
    al_draw_bitmap(ground, 0, CANVAS_HEIGHT-al_get_bitmap_height(ground), 0);

    al_flip_display();
}
