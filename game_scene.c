#include "resources.h"

void handle_click(int *scene)
{
    if (*scene == TITLE)
        *scene = INTRO;
    else if (*scene == GAMEOVER) {
        ALLEGRO_MOUSE_STATE m_state;
        al_get_mouse_state(&m_state);
        int x = m_state.x;

        if (x < CANVAS_WIDTH/2.0) // play again, reset
            *scene = INIT_GAME;
        else
            *scene = QUIT;
    }
}

void tick(game_state_struct *state, float audio_level, object *objects,
        ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, intro_resource_struct *intro_resources)
{
    al_clear_to_color(al_map_rgb(135, 206, 235));

    switch (state->scene) {
    TITLE:
        show_titlescreen(font, &objects[NEWTON]);
        break;
    INTRO:
        int finished = show_intro(objects, display, intro_resources);
        if (finished == 1)
            state->scene = INSTRUCTIONS;
        break;
    INSTRUCTIONS:
        int finished = show_instructions(objects, display, intro_resources);
        if (finished == 1) {
            // reset to current velocity when apple is destroyed
            objects[APPLE].reset_x_vel = objects[APPLE].x_vel;
            state->scene = INIT_GAME;
        }
        break;
    INIT_GAME:
        init_game(game_state, objects);
        state->scene = GAME;
        break;
    GAME:
        int gameover = game_tick(game_state, audio_level, objects);
        draw_game(game_state, objects, display, font);
        if (gameover == 1)
            state->scene = GAMEOVER;
        break;
    GAMEOVER:
        show_gameover(state->score, font);
        break;
    }

    al_flip_display();
}