#include "tick.h"

void handle_click(int *scene)
{
    if (*scene == TITLE)
        *scene = INIT_INTRO;
    else if (*scene == GAMEOVER) {
        ALLEGRO_MOUSE_STATE m_state;
        al_get_mouse_state(&m_state);
        int x = m_state.x;

        if (x < CANVAS_WIDTH/2.0) // play again, reset
            *scene = INIT_GAME_WITH_RESET;
        else
            *scene = QUIT;
    }
}

void tick(game_state_struct *game_state, 
        float audio_level, object *objects,
        ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, intro_resource_struct *intro_resources)
{
    int finished, gameover, cycle_finished;

    al_clear_to_color(al_map_rgb(135, 206, 235));

    switch (game_state->scene) {
    case INIT_TITLE:
        // Newton's state for the title screen
        objects[NEWTON].x_pos = CANVAS_WIDTH * 1.0/4;
        objects[NEWTON].y_pos = CANVAS_HEIGHT * 2.0/3;
        objects[NEWTON].x_vel = 2;
        game_state->scene = TITLE;
        game_state->scene_timer = 0;
        // drop through
    case TITLE:
        show_titlescreen(font, &objects[NEWTON], game_state->scene_timer);
        break;

    case INIT_INTRO:
        objects[APPLE].reset_x_vel = 0;
        reset_objects(objects);
        objects[TREE].x_pos = -30;
        objects[TREE].y_pos = objects[GROUND].y_pos - objects[TREE].height;
        game_state->scene = INTRO;
        game_state->scene_timer = 0;
        // drop through
    case INTRO:
        finished = show_intro(objects, display, intro_resources->font, game_state);
        if (finished == 1)
            game_state->scene = INIT_INSTRUCTIONS;
        break;

    case INIT_INSTRUCTIONS:
        game_state->scene = INSTRUCTIONS;
        game_state->scene_timer = 0;
        intro_resources->times_blown = 0;
    case INSTRUCTIONS:
        cycle_finished = show_instructions(objects, display,
            intro_resources->instructions1, intro_resources->instructions2,
            game_state);
        if (cycle_finished)
            intro_resources->times_blown++;
        if (intro_resources->times_blown == 3) {
            // reset to current velocity when apple is destroyed
            objects[APPLE].reset_x_vel = objects[APPLE].x_vel;
            game_state->scene = INIT_GAME;
        }
        break;

    // note fallthrough!
    case INIT_GAME_WITH_RESET:
        reset_objects(objects);
    case INIT_GAME:
        init_game(game_state, objects);
        game_state->scene = GAME;
    case GAME:
        gameover = game_tick(objects, audio_level, &(game_state->lives), &(game_state->score));
        draw_game(objects, display, font, game_state);
        if (gameover == 1)
            game_state->scene = GAMEOVER;
        break;

    case GAMEOVER:
        show_gameover(game_state->score, font);
        break;
    }

    al_flip_display();
    game_state->scene_timer++;
    game_state->anim_state.frame_n++;
}
