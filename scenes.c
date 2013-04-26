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

int show_intro(object *objects, int *tree_x, ALLEGRO_DISPLAY *display)
{
    static int t = 0;
    static int camera_vel = 0;
    static int sprite_n = 2;

    const int DROP_T = 60;
    const int GUST_T = DROP_T + 45;
    const int CAMERA_MOVE_T = GUST_T + 100;
    const int FINISH_T = CAMERA_MOVE_T + 80;

    ALLEGRO_BITMAP *newton_sprite;
    ALLEGRO_BITMAP *tree = al_load_bitmap("tree.png");

    int tree_height = al_get_bitmap_height(tree);
    //int tree_width = al_get_bitmap_width(tree);
    //int tree_x = objects[GROUND].x_pos;
    int tree_y = objects[GROUND].y_pos - tree_height;

    if (t == 0) {
        reset_object_physics(objects, APPLE);
        reset_object_physics(objects, NEWTON);
        objects[APPLE].x_pos = objects[NEWTON].x_pos;
        sprite_n = 2;  // asleep
        // TODO y pos?
    } if (t == DROP_T) {
        objects[APPLE].x_acc = 0;
        objects[APPLE].y_acc = 0.1;
        objects[APPLE].x_vel = 0;
        objects[APPLE].y_vel = 0;
    } else if (t == GUST_T) {
        objects[APPLE].y_acc = -0.2;
        objects[APPLE].x_acc = 0.3;
    } else if (t == CAMERA_MOVE_T) {
        camera_vel = APPLE_INIT_VEL * (int) round(pow(1.01, t-120));
    }

    if (t >= DROP_T && t <= CAMERA_MOVE_T)
        update_physics(objects, APPLE);

    if (t == GUST_T)
        sprite_n = 0;   // awake!
    if (t >= CAMERA_MOVE_T && t % ANIMATE_TIME == 0)
        sprite_n = !sprite_n;   // running!

    if (sprite_n == 0)
        newton_sprite = objects[NEWTON].sprite1;
    else if (sprite_n == 1)
        newton_sprite = objects[NEWTON].sprite2;
    else //if (sprite_n == 2)
        newton_sprite = objects[NEWTON].sprite3;

    al_draw_bitmap(objects[GROUND].sprite1, objects[GROUND].x_pos, objects[GROUND].y_pos, 0);
    al_draw_bitmap(tree, *tree_x, tree_y, 0);
    al_draw_bitmap(objects[APPLE].sprite1, objects[APPLE].x_pos, objects[APPLE].y_pos, 0);
    al_draw_bitmap(newton_sprite, objects[NEWTON].x_pos, objects[NEWTON].y_pos, 0);

    rotate_ground(objects[GROUND].sprite1, display, camera_vel);
    *tree_x = *tree_x - camera_vel;

    t++;

    if (t >= FINISH_T)
        return 1;
    else
        return 0;
}

int show_instructions()
{
    static int t = 0;

    if (t == 0)
        bitmap = 

    draw_objects(objects, (int) 100/objects[APPLE].x_vel);    // animate faster as apple goes faster
    al_draw_bitmap(bitmap, )
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
    reset_object_physics(objects, APPLE);
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
    draw_objects(objects, (int) 100/objects[APPLE].x_vel);    // animate faster as apple goes faster

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