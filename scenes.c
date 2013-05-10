#include "scenes.h"

void show_titlescreen(ALLEGRO_FONT *font, object *newton, int ticks)
{
    const int min_x = CANVAS_WIDTH * 1.0/4;
    const int max_x = CANVAS_WIDTH * 3.0/4;
    ALLEGRO_BITMAP *sprite;
    int font_line_height = al_get_font_line_height(font);

    if ((newton->x_vel > 0 && newton->x_pos == max_x) ||
        (newton->x_vel < 0 && newton->x_pos == min_x))
            newton->x_vel *= -1;

    newton->x_pos += newton->x_vel;

    // x % 10 ranges from 0 to 9
    // half is from 0 to 4, other half from 5 to 9
    if (ticks % 10 <= 4)
        sprite = newton->sprite1;
    else
        sprite = newton->sprite2;

    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH/2, CANVAS_HEIGHT * 1/3.0,
                 ALLEGRO_ALIGN_CENTRE,
                 "Newton's Apple");
    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH/2, CANVAS_HEIGHT * 1/3.0 + font_line_height,
                 ALLEGRO_ALIGN_CENTRE,
                 "Click to play!");

    al_draw_bitmap(sprite,
                   newton->x_pos, newton->y_pos,
                   newton->x_vel < 0 ? ALLEGRO_FLIP_HORIZONTAL : 0);   // flags
}

extern const float G;   // was defined in game.c
int show_intro(object *objects, ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, int ticks)
{
    enum times {
        LABEL_VANISH_T = 100,
        DROP_T = 220,
        GUST_T = DROP_T + 45,
        PUZZLE_START_T = GUST_T + 100,
        PUZZLE_END_T = PUZZLE_START_T + 60,
        CAMERA_MOVE_T = PUZZLE_END_T + 60
    };

    const int APPLE_OFFSET_X = 100;
    const int APPLE_OFFSET_Y = 120;
    static int apple_was_offscreen = 0;
    int apple_is_offscreen, apple_is_onscreen;

    object *newton = &objects[NEWTON];
    object *tree = &objects[TREE];
    const int CAMERA_INIT_VEL = 5;
    const float CAMERA_SPEEDUP_K = 1.01;
    const float CAMERA_SLOWDOWN_K = 0.13; // derived experimentally :(

    int newton_label_x = objects[NEWTON].x_pos;
    int newton_label_y = objects[NEWTON].y_pos +
        al_get_bitmap_height(objects[NEWTON].sprite1) + 10;
    int puzzle_label_x = objects[NEWTON].x_pos +
        al_get_bitmap_width(objects[NEWTON].sprite1) + 10;
    int puzzle_label_y = objects[NEWTON].y_pos + 10;


    // step 1: figure out what the apple's doing
    {
        if (ticks == 0) {
            object *a = &objects[APPLE];
            a->x_vel = a->y_vel = a->x_acc = a->y_acc = 0;
            objects[APPLE].x_pos = APPLE_OFFSET_X;
            objects[APPLE].y_pos = APPLE_OFFSET_Y;
        } if (ticks == DROP_T) {
            objects[APPLE].x_acc = 0;
            objects[APPLE].y_acc = 0.1;
            objects[APPLE].x_vel = 0;
            objects[APPLE].y_vel = 0;
        } else if (ticks == GUST_T) {
            objects[APPLE].y_acc = -0.2;
            objects[APPLE].x_acc = 0.3;
        }
        apple_is_offscreen = (check_if_offscreen(&objects[APPLE]) == 1);
        apple_is_onscreen = !apple_is_offscreen;
        if (apple_is_offscreen) {
            objects[APPLE].y_pos = INIT_APPLE_Y;       // in the right place when the game starts
            objects[APPLE].y_vel = 0;
            objects[APPLE].y_acc = 0;
            // x_vel stays the same after the gust
            objects[APPLE].x_acc = 0;
            apple_was_offscreen = 1;
        }
    }

    // step 2: figure out what the camera should be doing
    {
        if (ticks == CAMERA_MOVE_T)
            newton->x_vel = CAMERA_INIT_VEL;
        // if before newton catching up to apple, accelerate
        if (newton->x_vel > 0 && apple_is_offscreen)
            newton->x_vel *= CAMERA_SPEEDUP_K;
        // once caught up, use negative feedback to sync with apple speed
        else if (newton->x_vel > 0 && apple_is_onscreen)
            newton->x_vel -= CAMERA_SLOWDOWN_K*(newton->x_vel - objects[APPLE].x_vel);
    }

    // step 3: update physics
    {
        // so this updates the absolute position of the apple
        update_physics(objects, APPLE, MODE_ABSOLUTE);
        // now do another update considering the scene movement
        objects[APPLE].x_pos -= newton->x_vel;
        // once the apple has come back onscreen, (after Newton runs after it),
        // check it hasn't gone past where it should be
        if (newton->x_vel > 0 && objects[APPLE].x_pos <= INIT_APPLE_X)
            objects[APPLE].x_pos = INIT_APPLE_X;

        rotate_ground(&objects[GROUND], display, (int) round(newton->x_vel));
        tree->x_pos -= newton->x_vel;
    }

    // step 4: draw the scene
    {
        if (ticks >= CAMERA_MOVE_T)
            draw_objects_with_animate(objects, newton->x_vel); // running!
        else {
            draw_object_sprite_n(&objects[GROUND], 1);
            draw_object_sprite_n(&objects[APPLE], 1);
            if (ticks > GUST_T) // awake!
                draw_object_sprite_n(newton, 1);
            else    // still asleep
                draw_object_sprite_n(newton, 3);
        }

        draw_object_sprite_n(tree, 1);

        if (ticks <= LABEL_VANISH_T) {
            al_draw_text(font, al_map_rgb(255,255,255),
                         newton_label_x, newton_label_y,
                         ALLEGRO_ALIGN_LEFT,
                         "Isaac Newton");
        }
        if (ticks >= PUZZLE_START_T && ticks <= PUZZLE_END_T) {
            al_draw_text(font, al_map_rgb(255,255,255),
                         puzzle_label_x, puzzle_label_y,
                         ALLEGRO_ALIGN_LEFT,
                         "?");
        }
    }

    if (apple_was_offscreen &&
            objects[APPLE].x_pos == INIT_APPLE_X) {
        newton->x_vel = 0; // ready for main game scene
        return 1;   // finished
    } else
        return 0;
}

extern const float G;   // defined in physics.c
int show_instructions(object *objects, ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *instructions1, ALLEGRO_BITMAP *instructions2)
{
    static int t = 0;
    static int times_blown = 0;
    ALLEGRO_BITMAP *instructions = instructions1; // by default

    const int PAUSE_T = 60;

    if (t == PAUSE_T)
        objects[APPLE].y_acc = G;

    if (t >= PAUSE_T &&
            objects[APPLE].y_pos > CANVAS_HEIGHT * 0.4) {   // too low!
        objects[APPLE].y_acc = -G;    // bloooow!
        instructions = instructions2;
    } else if (t >= PAUSE_T &&
            objects[APPLE].y_pos < CANVAS_HEIGHT * 0.4) {  // high enough, stop blowing
        if (objects[APPLE].y_acc != G) // we've just switched
            times_blown++;
        objects[APPLE].y_acc = G;
        instructions = instructions1;
    }

    update_physics(objects, APPLE, MODE_WRT_APPLE);
    rotate_ground(&objects[GROUND], display, objects[APPLE].x_vel);
    draw_objects_with_animate(objects, objects[APPLE].x_vel);    // second arg animate interval - faster as apple goes faster
    if (t > PAUSE_T)
        al_draw_bitmap(instructions, CANVAS_WIDTH * 4/6.0, CANVAS_HEIGHT/4.0, 0);

    t++;

    if (times_blown == 3)
        return 1;
    else
        return 0;
}

void init_game(game_state_struct *game_state, object *objects)
{
    game_state->score = 0;
    game_state->lives = 3;
    enum object_ctr i;
    for (i = 0; i < LAST_MOVER; i++) {
        // give the player some time to get used to things
        if (i == BIRD || i == PROJECTILE) {
            objects[i].destroyed = 1;
            load_respawn(&objects[i], i);
        }
    }
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
                 ALLEGRO_ALIGN_CENTRE, "Play again?");

    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH * 2.0/6, CANVAS_HEIGHT/2 + font_line_height,
                 ALLEGRO_ALIGN_CENTRE, "Yeah!");
    al_draw_text(font, al_map_rgb(255,255,255),
                 CANVAS_WIDTH * 4.0/6, CANVAS_HEIGHT/2 + font_line_height,
                 ALLEGRO_ALIGN_CENTRE, "Quit");
}
