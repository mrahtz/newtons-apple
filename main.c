#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <portaudio.h>
#include <math.h>
#include <stdlib.h>

#include "globals.h"
#include "game.h"
#include "helpers.h"

const float G = 0.1;
const int CANVAS_WIDTH = 640;
const int CANVAS_HEIGHT = 480;
const int PROJ_INTERVAL = 10;
const int MIN_PROJ_VEL = 5;
const int MAX_PROJ_VEL = 10;
const int ANIMATE_TIME = 30;

enum game_scenes {
    TITLE,
    INTRO,
    GAME,
    GAMEOVER
};

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
    ALLEGRO_BITMAP *ground = NULL;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *font;
    PaStream *audio_stream;
    float audio_level = 0;
    object objects[3];
    int animate_timer = 0;
    int score = 0;
    int lives = 1;
    int scene = GAME;
    int font_line_height;

    // initialisation
    {
        enum object_ctr i;

        al_init();
        al_init_image_addon();
        al_init_font_addon();
        al_init_ttf_addon();
        for (i = 0; i < LAST_OBJECT; i++) {
            init_object(&objects[i], i);
            objects[i].sprite_n = 0;
            objects[i].sprite1 = al_load_bitmap("newton1.png");
            objects[i].sprite2 = al_load_bitmap("newton2.png");
            if (!objects[i].sprite1 || !objects[i].sprite2)
                die("couldn't load sprites for objects[%d]\n", i);
        }
        ground = al_load_bitmap("ground.png");
        font = al_load_ttf_font("Arial.ttf", 36, 0);    // last arg is flags
        font_line_height = al_get_font_line_height(font);
        if (!font)
            die("couldn't load font\n");

        display = al_create_display(CANVAS_WIDTH, CANVAS_HEIGHT);

        /*if (!newton) {
            printf("oh no!\n");
            return 1;
        }*/

        event_queue = al_create_event_queue();
        al_register_event_source(event_queue, al_get_display_event_source(display));
        // TODO free this timer?
        timer = al_create_timer(1/60.0);
        if (!timer)
            die("couldn't create timer\n");
        al_register_event_source(event_queue, al_get_timer_event_source(timer));
        al_start_timer(timer);

        audio_stream = portaudio_init(record_callback, &audio_level);
    }

    // game loop
    while (1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        char score_str[20], lives_str[20];

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (scene == TITLE)
                ;// do stuff
            else if (scene == GAME) {   // main game
                if (simulate_world(objects, audio_level))
                    lives--;
                if (lives == 0) {
                    scene = GAMEOVER;
                    continue;
                }
                rotate_ground(ground, display, objects[APPLE].x_vel);
                draw_world(objects, ground, animate_timer);
                animate_timer++;

                score += (int) round(objects[APPLE].x_vel);
                sprintf(score_str, "Score: %d", score);
                al_draw_text(font, al_map_rgb(255,255,255),
                             10, 10,
                             ALLEGRO_ALIGN_LEFT, score_str);

                sprintf(lives_str, "Lives left: %d", lives);
                al_draw_text(font, al_map_rgb(255,255,255),
                             10, 10+font_line_height,
                             ALLEGRO_ALIGN_LEFT, lives_str);
            } else if (scene == GAMEOVER) {
                sprintf(score_str, "Final score: %d", score);

                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(font, al_map_rgb(255,255,255),
                             CANVAS_WIDTH/2, CANVAS_HEIGHT/2 - font_line_height,
                             ALLEGRO_ALIGN_CENTRE, score_str);
                al_draw_text(font, al_map_rgb(255,255,255),
                             CANVAS_WIDTH/2, CANVAS_HEIGHT/2,
                             ALLEGRO_ALIGN_CENTRE, "Play again? :)");

                al_draw_text(font, al_map_rgb(255,255,255),
                             CANVAS_WIDTH * 2.0/6, CANVAS_HEIGHT/2 + 2*font_line_height,
                             ALLEGRO_ALIGN_CENTRE, "Ja!");
                al_draw_text(font, al_map_rgb(255,255,255),
                             CANVAS_WIDTH * 4.0/6, CANVAS_HEIGHT/2 + 2*font_line_height,
                             ALLEGRO_ALIGN_CENTRE, "Nein!");
            }
            
            al_flip_display();
        }
    }
    
    free_resources(display, objects, event_queue, audio_stream);

    return 0;
}
