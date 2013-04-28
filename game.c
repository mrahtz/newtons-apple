#include "game.h"

const float G = 0.1;

void init_sprite(object *o, int object_n)
{
    char sprite1_fn[20], sprite2_fn[20];

    // so can call al_destroy_bitmap on them later,
    // even if they're unused
    o->sprite1 = o->sprite2 = o->sprite3 = NULL;

    switch (object_n) {
        case APPLE:
            strcpy(sprite1_fn, "apple.png");
            strcpy(sprite2_fn, "apple.png");
            break;
        case BIRD:
            strcpy(sprite1_fn, "bird1.png");
            strcpy(sprite2_fn, "bird2.png");
            break;
        case PROJECTILE:
            strcpy(sprite1_fn, "book.png");
            strcpy(sprite2_fn, "book.png");
            break;
        case NEWTON:
            strcpy(sprite1_fn, "newton1.png");
            strcpy(sprite2_fn, "newton2.png");
            o->sprite3 = al_load_bitmap("newton_asleep.png");
            //break;
    }

    o->sprite1 = al_load_bitmap(sprite1_fn);
    o->sprite2 = al_load_bitmap(sprite2_fn);
    if (!o->sprite1 || !o->sprite2)
        die("couldn't load sprites for objects[%d]\n", object_n);
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

int check_if_offscreen(const object *o)
{
    int o_width = al_get_bitmap_width(o->sprite1);
    int o_height = al_get_bitmap_height(o->sprite1);

    int offscreen_x = o->x_pos > CANVAS_WIDTH-1 ||
                      o->x_pos - 1 + o_width < 0;
    int offscreen_y = o->y_pos > CANVAS_HEIGHT-1 ||
                      o->y_pos - 1 + o_height < 0;

    return offscreen_x || offscreen_y;
}

int check_collision(const object *o1, const object *o2)
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

// returns 1 if apple was hit, otherwise 0
int simulate_objects(object *objects, const float audio_level)
{
    enum object_ctr i;

    for (i = 0; i < LAST_MOVER; i++) {
        object *o = &objects[i];

        // handle respawn timing
        if  (o->timer != 0)
            o->timer -= 1;
        else if (o->destroyed && o->timer == 0) {   // time to respawn
            o->timer = 0;
            o->destroyed = 0;
            reset_object_physics(objects, i);
        }
        if (o->destroyed)
            continue;

        if (i == APPLE)
            o->y_acc = G - 5*audio_level;

        update_physics(objects, i, MODE_WRT_APPLE);
        //printf("%f %f\n", o->x_pos, o->y_pos);

        if (i != APPLE &&
                !objects[APPLE].destroyed &&
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

void rotate_ground(ALLEGRO_BITMAP *ground, ALLEGRO_DISPLAY *display, int amount)
{
    int width = al_get_bitmap_width(ground);
    int height = al_get_bitmap_height(ground);
    ALLEGRO_COLOR *saved;

    // if rotating more than the width,
    // only rotate the different between the amount and the width
    amount %= width+1;

    al_lock_bitmap(ground,
                   ALLEGRO_PIXEL_FORMAT_ANY,    // pixel format
                   ALLEGRO_LOCK_READWRITE);     // mode
    al_set_target_bitmap(ground);
    saved = malloc(sizeof(ALLEGRO_COLOR) * amount);

    int y, x;
    for (y = 0; y < height; y++) {
        // save the pixels on the left, to rotate round later
        for (x = 0; x < amount; x++)
            saved[x] = al_get_pixel(ground, x, y);
        // shift pixels to the left
        for (x = 0; x < width-amount; x++)
            al_put_pixel(x, y, al_get_pixel(ground, x+amount, y));
        // stick the saved first pixel back on the end
        for (x = width-amount; x < width; x++)
            al_put_pixel(x, y, saved[x-(width-amount)]);
    }

    al_unlock_bitmap(ground);

    al_set_target_backbuffer(display);
}

void draw_objects_with_animate(const object *objects, int animate_time)
{
    static int animate_timer = 0;
    static int last_switch = 0;
    static int sprite_n = 1;

    enum object_ctr i;

    if (animate_time != 0 &&    // 0 -> don't animate
            animate_timer - last_switch >= animate_time) {
        last_switch = animate_timer;

        if (sprite_n == 1)
            sprite_n = 2;
        else
            sprite_n = 1;
    }

    for (i = 0; i < LAST_MOVER; i++)
        draw_object_sprite_n(&objects[i], sprite_n);
    al_draw_bitmap(objects[GROUND].sprite1, objects[GROUND].x_pos, objects[GROUND].y_pos, 0);

    animate_timer++;
}

void draw_object_sprite_n(const object *o, int sprite_n)
{
    if (o->destroyed)
        return;

    if (sprite_n == 1) {
        // dx, dy, flags (delta because relative to current postion)
        al_draw_bitmap(o->sprite1, (int) o->x_pos, (int) o->y_pos, 0);
    } else if (sprite_n == 2) 
        al_draw_bitmap(o->sprite2, (int) o->x_pos, (int) o->y_pos, 0);
    else if (sprite_n == 3)
        al_draw_bitmap(o->sprite3, (int) o->x_pos, (int) o->y_pos, 0);
}
