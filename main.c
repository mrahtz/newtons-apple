#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <portaudio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"
#include "scenes.h"
#include "game.h"
#include "helpers.h"

// stores average amplitude of buffer in userData pointer
int record_callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userdata)
{
    int i;
    float *in = (float *) inputBuffer;
    float *mean = (float *) userdata;
    float sum = 0;

    for (i = 0; i < framesPerBuffer; i++) {
        sum += fabs(in[0]);
    }
    *mean = sum/framesPerBuffer;

    return 0;
}

int main(void)
{
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *font;
    PaStream *audio_stream;
    ALLEGRO_BITMAP *instructions1, *instructions2;
    object objects[OBJECTS_END];
    float audio_level = 0;
    int scene = INTRO;
    int score;
    int lives;
    int tree_x = 0;

    // initialisation
    {
        al_init();
        al_init_image_addon();
        al_init_font_addon();
        al_init_ttf_addon();
        al_install_mouse();
        srand(time(NULL));

        display = al_create_display(CANVAS_WIDTH, CANVAS_HEIGHT);

        // though the window pops up for a second with nothing on it,
        // they have to be loaded after display creation for them to be
        // loaded into video memory (rather than system memory)
        enum object_ctr i;
        for (i = 0; i < LAST_OBJECT; i++) {
            init_sprite(&objects[i], i);
            load_respawn(&objects[i], i);
        }
        objects[GROUND].sprite1 = al_load_bitmap("ground.png");
        objects[GROUND].x_pos = 0;
        objects[GROUND].y_pos = CANVAS_HEIGHT-1 -
                                al_get_bitmap_height(objects[GROUND].sprite1) + 1;

        instructions1 = al_load_bitmap("instructions1.png");
        instructions2 = al_load_bitmap("instructions2.png");

        font = al_load_ttf_font("Arial.ttf",
                                36,     // size
                                0);     // flags
        if (!font)
            die("couldn't load font\n");

        event_queue = al_create_event_queue();
        // TODO free this timer?
        timer = al_create_timer(1/60.0);
        if (!timer)
            die("couldn't create timer\n");
        al_start_timer(timer);

        al_register_event_source(event_queue, al_get_display_event_source(display));
        al_register_event_source(event_queue, al_get_timer_event_source(timer));
        al_register_event_source(event_queue, al_get_mouse_event_source());

        audio_stream = portaudio_init(record_callback, &audio_level);
        //audio_stream = NULL;

        init_game(objects, &lives, &score);
    }

    // game loop
    while (1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            puts("break?");
            break;
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            puts("mouse event");
            if (scene == TITLE)
                scene = INTRO;
            else if (scene == GAMEOVER) {
                ALLEGRO_MOUSE_STATE m_state;
                al_get_mouse_state(&m_state);
                int x = m_state.x;

                if (x < CANVAS_WIDTH/2.0) { // play again, reset
                    init_game(objects, &lives, &score);
                    scene = GAME;
                } else // quit
                    break;
            }
        } else if (ev.type == ALLEGRO_EVENT_TIMER) {    // time for next frame to be drawn
            al_clear_to_color(al_map_rgb(135, 206, 235));

            if (scene == TITLE)
                show_titlescreen(font, &objects[3]);
            else if (scene == INTRO) {
                int finished = show_intro(objects, &tree_x, display);
                if (finished == 1)
                    scene = INSTRUCTIONS;
            } else if (scene == INSTRUCTIONS) {
                int finished = show_instructions(objects, instructions1, instructions2);
                if (finished == 1) {
                    init_game(objects, &lives, &score);
                    scene = GAME;
                }
            } else if (scene == GAMEOVER)
                show_gameover(score, font);
            else if (scene == GAME) {   // main game
                printf("%f %f\n", objects[NEWTON].x_pos, objects[NEWTON].y_pos);
                int gameover = game_tick(objects, audio_level, &lives, &score);
                if (gameover == 1)
                    scene = GAMEOVER;
                draw_game(objects, display, font, score, lives);
            }

            al_flip_display();
        }
    }

    free_resources(display, objects, event_queue, audio_stream);

    return 0;
}
