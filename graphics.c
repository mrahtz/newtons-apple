#include "graphics.h"

void rotate_ground(object *ground, ALLEGRO_DISPLAY *display, int amount)
{
    int width = ground->width;
    int height = ground->height;
    ALLEGRO_BITMAP *bm = ground->sprite1;
    ALLEGRO_COLOR *saved;

    // if rotating more than the width,
    // only rotate the different between the amount and the width
    amount %= width+1;

    al_lock_bitmap(bm,
                   ALLEGRO_PIXEL_FORMAT_ANY,    // pixel format
                   ALLEGRO_LOCK_READWRITE);     // mode
    al_set_target_bitmap(bm);
    saved = malloc(sizeof(ALLEGRO_COLOR) * amount);

    int y, x;
    for (y = 0; y < height; y++) {
        // save the pixels on the left, to rotate round later
        for (x = 0; x < amount; x++)
            saved[x] = al_get_pixel(bm, x, y);
        // shift pixels to the left
        for (x = 0; x < width-amount; x++)
            al_put_pixel(x, y, al_get_pixel(bm, x+amount, y));
        // stick the saved first pixel back on the end
        for (x = width-amount; x < width; x++)
            al_put_pixel(x, y, saved[x-(width-amount)]);
    }

    free(saved);

    al_unlock_bitmap(bm);

    al_set_target_backbuffer(display);
}

void draw_objects_with_animate(object *objects, int velocity)
{
    static int animate_timer = 0;
    static int last_switch = 0;
    static int sprite_n = 1;

    enum object_ctr i;

    int animate_time = (int) 300/velocity;
    if (animate_time != 0 &&    // 0 -> don't animate
            animate_timer - last_switch >= animate_time) {
        last_switch = animate_timer;

        if (sprite_n == 1)
            sprite_n = 2;
        else
            sprite_n = 1;
    }

    for (i = 0; i < LAST_MOVER; i++) {
        if (! objects[i].destroyed)
            draw_object_sprite_n(&objects[i], sprite_n);
    }
    draw_object_sprite_n(&objects[GROUND], 1);

    animate_timer++;
}

void draw_object_sprite_n(object *o, int sprite_n)
{
    if (sprite_n == 1) {
        al_draw_bitmap(o->sprite1, (int) o->x_pos, (int) o->y_pos, 0);
    } else if (sprite_n == 2) 
        al_draw_bitmap(o->sprite2, (int) o->x_pos, (int) o->y_pos, 0);
    else if (sprite_n == 3)
        al_draw_bitmap(o->sprite3, (int) o->x_pos, (int) o->y_pos, 0);
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
