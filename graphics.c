#include "graphics.h"

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
    draw_object_sprite_n(&objects[GROUND], 1);

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
