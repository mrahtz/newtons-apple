#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
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
    PaStream *audio_stream;
    float audio_level = 0;
    object objects[3];
    int animate_timer = 0;

    // initialisation
    {
        enum object_ctr i;

        al_init();
        al_init_image_addon();
        for (i = 0; i < LAST_OBJECT; i++) {
            reset_object(&objects[i], i);
            objects[i].sprite_n = 0;
            objects[i].sprite1 = al_load_bitmap("newton1.png");
            objects[i].sprite2 = al_load_bitmap("newton2.png");
            if (!objects[i].sprite1 || !objects[i].sprite2)
                die("couldn't load sprites for objects[%d]\n", i);
        }
        ground = al_load_bitmap("ground.png");

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

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            simulate_world(objects, audio_level);
            draw_world(objects, ground, animate_timer);

            animate_timer++;
        }
    }
    
    free_resources(display, objects, event_queue, audio_stream);

    return 0;
}
