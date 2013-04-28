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
                   x, CANVAS_HEIGHT * 4.0/6,
                   direction == 1 ? ALLEGRO_FLIP_HORIZONTAL : 0);   // flags

    animate_timer++;
}

extern const float G;   // was defined in game.c
int show_intro(object *objects, ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *tree)
{
    static int t = 0;
    static float camera_vel = 0;
    static int sprite_n = 2;
    static int apple_was_offscreen = 0;
    static int tree_x = -30;
    int tree_height = al_get_bitmap_height(tree);
    int tree_y = objects[GROUND].y_pos - tree_height;

    const int DROP_T = 120;
    const int GUST_T = DROP_T + 45;
    const int CAMERA_MOVE_T = GUST_T + 100;
    const int APPLE_OFFSET_X = 100;
    const float CAMERA_SLOWDOWN_K = 0.13; // derived experimentally :(

    ALLEGRO_BITMAP *newton_sprite;

    if (t == 0) {
        reset_object_physics(objects, APPLE);
        reset_object_physics(objects, NEWTON);
        objects[APPLE].x_pos = APPLE_OFFSET_X;
        sprite_n = 2;  // asleep
    } if (t == DROP_T) {
        objects[APPLE].x_acc = 0;
        objects[APPLE].y_acc = 0.1;
        objects[APPLE].x_vel = 0;
        objects[APPLE].y_vel = 0;
    } else if (t == GUST_T) {
        objects[APPLE].y_acc = -0.2;
        objects[APPLE].x_acc = 0.3;
    } else if (t == CAMERA_MOVE_T) {
        camera_vel = (float) APPLE_INIT_VEL; // for want of a better value
    }

    int apple_is_offscreen = (check_if_offscreen((const object *) &objects[APPLE]) == 1);
    if (apple_is_offscreen) {
        objects[APPLE].y_pos = INIT_APPLE_Y;       // in the right place when the game starts
        objects[APPLE].y_vel = 0;
        objects[APPLE].x_acc = 0;
        objects[APPLE].y_acc = 0;
        // x_vel stays the same after the gust
        apple_was_offscreen = 1;
    }

    // if before newton catching up to apple, acclerate
    int apple_is_onscreen = !apple_is_offscreen;
    if ( ! (apple_was_offscreen && apple_is_onscreen))
        camera_vel *= 1.015;
    else // sync with apple speed
        camera_vel -= CAMERA_SLOWDOWN_K*(camera_vel - objects[APPLE].x_vel);

    if (t >= DROP_T) {
        // so this updates the absolute position of the apple
        update_physics(objects, APPLE, MODE_ABSOLUTE);
        // now do another update considering the scene movement
        objects[APPLE].x_pos -= camera_vel;
        if (apple_was_offscreen && objects[APPLE].x_pos <= INIT_APPLE_X)    // gone a little too far...
            objects[APPLE].x_pos = INIT_APPLE_X;
    }

    if (t == GUST_T)
        sprite_n = 0;   // awake!
    if (t >= CAMERA_MOVE_T && t % ANIMATE_TIME == 0)
        sprite_n = !sprite_n;   // running!

    if (sprite_n == 0)
        newton_sprite = objects[NEWTON].sprite1; // running 1
    else if (sprite_n == 1)
        newton_sprite = objects[NEWTON].sprite2; // running 2
    else //if (sprite_n == 2)
        newton_sprite = objects[NEWTON].sprite3; // asleep

    al_draw_bitmap(objects[GROUND].sprite1, objects[GROUND].x_pos, objects[GROUND].y_pos, 0);
    al_draw_bitmap(tree, tree_x, tree_y, 0);
    al_draw_bitmap(objects[APPLE].sprite1, objects[APPLE].x_pos, objects[APPLE].y_pos, 0);
    al_draw_bitmap(newton_sprite, objects[NEWTON].x_pos, objects[NEWTON].y_pos, 0);

    rotate_ground(objects[GROUND].sprite1, display, (int) round(camera_vel));
    tree_x = tree_x - camera_vel;

    t++;

    if (apple_was_offscreen &&
            objects[APPLE].x_pos == INIT_APPLE_X)
        return 1;
    else
        return 0;
}

extern const float G;   // defined in game.c
int show_instructions(object *objects, ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *instructions1, ALLEGRO_BITMAP *instructions2)
{
    static int t = 0;
    static int loop = 0;
    ALLEGRO_BITMAP *bm = instructions1; // by default


    const int PAUSE_T = 60;
    // pause
    // fall
    // if too low, blow
    // if too high, stop
    // if too low, blow
    // if too high, stop

    if (t == PAUSE_T)
        objects[APPLE].y_acc = G;

    if (t >= PAUSE_T &&
            objects[APPLE].y_pos > CANVAS_HEIGHT * 0.4) {   // too low!
        objects[APPLE].y_acc = -G;    // bloooow!
        bm = instructions2;
    } else if (t >= PAUSE_T &&
            objects[APPLE].y_pos < CANVAS_HEIGHT * 0.4) {  // high enough, stop blowing
        if (objects[APPLE].y_acc != G) // we've just switched
            loop++;
        objects[APPLE].y_acc = G;
        bm = instructions1;
    }

    update_physics(objects, APPLE, MODE_WRT_APPLE);
    rotate_ground(objects[GROUND].sprite1, display, objects[APPLE].x_vel);
    draw_objects(objects, (int) 100/objects[APPLE].x_vel);    // second arg animate interval - faster as apple goes faster
    if (t > PAUSE_T)
        al_draw_bitmap(bm, CANVAS_WIDTH * 4/6.0, CANVAS_HEIGHT/4.0, 0);
    al_draw_bitmap(objects[GROUND].sprite1, objects[GROUND].x_pos, objects[GROUND].y_pos, 0);

    t++;

    if (loop == 3)
        return 1;
    else
        return 0;
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
            objects[i].timer = (int) INIT_SPAWN_INTERVAL * rand_between(1.0, 3.0);
        }
    }
    objects[APPLE].destroyed = 0;
    objects[APPLE].timer = 0;
    objects[NEWTON].destroyed = 0;
    //reset_object_physics(objects, APPLE);
    reset_object_physics(objects, NEWTON);
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
    draw_objects(objects, (int) 100/objects[APPLE].x_vel);    // second arg animate interval - faster as apple goes faster

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
