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
    object objects[LAST_OBJECT];
    float audio_level = 0;
    int scene = TITLE;
    int font_line_height;
    int score;
    int lives;

    // initialisation
    {
        al_init();
        al_init_image_addon();
        al_init_font_addon();
        al_init_ttf_addon();
        al_install_mouse();
        srand(time(NULL));

        enum object_ctr i;
        for (i = 0; i < LAST_OBJECT; i++) {
            init_sprite(&objects[i], i);
            load_respawn(&objects[i], i);
        }
        objects[GROUND].sprite1 = al_load_bitmap("ground.png");
        objects[GROUND].x_pos = 0;
        objects[GROUND].y_pos = CANVAS_HEIGHT-1 - 
                                al_get_bitmap_height(objects[GROUND].sprite1) + 1;

        font = al_load_ttf_font("Arial.ttf",
                                36,     // size
                                0);     // flags
        if (!font)
            die("couldn't load font\n");
        font_line_height = al_get_font_line_height(font);

        display = al_create_display(CANVAS_WIDTH, CANVAS_HEIGHT);
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

        init_game(objects, &lives, &score);
    }

    // game loop
    while (1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (scene == TITLE)
                scene = GAME;
            else if (scene == GAMEOVER) {
                ALLEGRO_MOUSE_STATE m_state;
                al_get_mouse_state(&m_state);
                int x = m_state.x;

                if (x < CANVAS_WIDTH/2.0) { // play again, reset
                    init_game(objects, &lives, &score);
                    scene = GAME;
                } else
                    break;
            }
        } else if (ev.type == ALLEGRO_EVENT_TIMER) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (scene == TITLE)
                show_titlescreen(font, &objects[3]);
            else if (scene == GAMEOVER)
                show_gameover(score, font);
            else if (scene == GAME) {   // main game
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
