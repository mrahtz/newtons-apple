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

void draw_objects_with_animate(object *objects, int velocity, int ticks)
{
    static int last_switch = 0;
    static int sprite_n = 1;

    enum object_ctr i;

    int animate_time;
    if (velocity == 0)
        animate_time = 0;
    else
        animate_time = (int) 300/velocity;
    if (animate_time != 0 &&    // 0 -> don't animate
            ticks - last_switch >= animate_time) {
        last_switch = ticks;

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

// al_draw_bitmap doesn't take consts so can't const their parameters :(
void draw_game(object *objects,
               ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font,
               const game_state_struct *game_state)
{
    char lives_str[20], score_str[20];
    int font_line_height = al_get_font_line_height(font);

    rotate_ground(&objects[GROUND], display, objects[APPLE].x_vel);
    // second arg animate interval - faster as apple goes faster
    draw_objects_with_animate(objects, objects[APPLE].x_vel, game_state->ticks);
    sprintf(score_str, "Score: %d", game_state->score);
    al_draw_text(font, al_map_rgb(255,255,255),
                 10, 10,
                 ALLEGRO_ALIGN_LEFT, score_str);

    sprintf(lives_str, "Lives left: %d", game_state->lives);
    al_draw_text(font, al_map_rgb(255,255,255),
                 10, 10+font_line_height,
                 ALLEGRO_ALIGN_LEFT, lives_str);
}
