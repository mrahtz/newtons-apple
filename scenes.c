#include "scenes.h"

void show_titlescreen(ALLEGRO_FONT *font, object *newton)
{
    const int min_x = CANVAS_WIDTH * 1.0/4;
    const int max_x = CANVAS_WIDTH * 3.0/4;
    static int x = CANVAS_WIDTH * 1.0/4;
    static int direction = 0;   // 0 = right, 1 = left
    static int animate_timer = 0;
    static int sprite_n = 0;
    ALLEGRO_BITMAP *sprite;
    int font_line_height = al_get_font_line_height(font);

    if (direction == 0 && x == max_x)
        direction = !direction;
    else if (direction == 1 && x == min_x)
        direction = !direction;

    if (direction == 0)
        x += 2;
    else
        x -= 2;

    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH/2, CANVAS_HEIGHT * 1/3.0,
                 ALLEGRO_ALIGN_CENTRE,
                 "Newton's Apple");
    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH/2, CANVAS_HEIGHT * 1/3.0 + font_line_height,
                 ALLEGRO_ALIGN_CENTRE,
                 "Click to play!");

    if (animate_timer % 5 == 0)
        sprite_n = !sprite_n;
    if (sprite_n == 0)
        sprite = newton->sprite1;
    else
        sprite = newton->sprite2;
    al_draw_bitmap(sprite,
                   x, CANVAS_HEIGHT * 3.0/4,
                   0);

    animate_timer++;
}

void init_game(object *objects, int *lives, int *score)
{
    *score = 0;
    *lives = 3;
    enum object_ctr i;
    for (i = 0; i < LAST_OBJECT; i++) {
        // give the player some time to get used to things
        if (i != APPLE && i != NEWTON) {
            objects[i].destroyed = 1;
            objects[i].timer = (int) INIT_SPAWN_INTERVAL * rand_between(1.0, 3.0);     // TODO randomise
        }
    }
    objects[APPLE].destroyed = 0;
    objects[APPLE].timer = 0;
    reset_object_physics(&objects[APPLE], APPLE);
    reset_object_physics(&objects[NEWTON], NEWTON);
}

int game_tick(object *objects, const float audio_level, int *lives, int *score)
{
    int hit = simulate_objects(objects, audio_level);
    if (hit)
        (*lives)--;
    if (*lives == 0)
        return 1;

    *score += (int) objects[APPLE].x_vel;

    return 0;
}

// al_draw_bitmap doesn't take consts so can't const their parameters :(
void draw_game(const object *objects,
               ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font,
               const int score, const int lives)
{
    char lives_str[20], score_str[20];
    int font_line_height = al_get_font_line_height(font);

    rotate_ground(objects[GROUND].sprite1, display, objects[APPLE].x_vel);
    al_draw_bitmap(objects[GROUND].sprite1, objects[GROUND].x_pos, objects[GROUND].y_pos, 0);
    // TODO remove the offset?
    draw_objects(objects, 0);

    sprintf(score_str, "Score: %d", score);
    al_draw_text(font, al_map_rgb(255,255,255),
                 10, 10,
                 ALLEGRO_ALIGN_LEFT, score_str);

    sprintf(lives_str, "Lives left: %d", lives);
    al_draw_text(font, al_map_rgb(255,255,255),
                 10, 10+font_line_height,
                 ALLEGRO_ALIGN_LEFT, lives_str);
}

void show_gameover(int score, ALLEGRO_FONT *font)
{
    char score_str[20];
    int font_line_height = al_get_font_line_height(font);

    sprintf(score_str, "Final score: %d", score);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH/2, CANVAS_HEIGHT/2 - 2*font_line_height,
                 ALLEGRO_ALIGN_CENTRE, score_str);
    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH/2, CANVAS_HEIGHT/2 - font_line_height,
                 ALLEGRO_ALIGN_CENTRE, "Play again? :)");

    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH * 2.0/6, CANVAS_HEIGHT/2 + font_line_height,
                 ALLEGRO_ALIGN_CENTRE, "Ja!");
    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH * 4.0/6, CANVAS_HEIGHT/2 + font_line_height,
                 ALLEGRO_ALIGN_CENTRE, "Nein!");
}