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
#include "utilities.h"
#include "resources.h"
#include "audio.h"
#include "tick.h"

ALLEGRO_DISPLAY * init_allegro(ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_TIMER **timer)
{
    ALLEGRO_DISPLAY *display;

    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_mouse();

    // creating the display before loading the sprites produces a slight pause,
    // but otherwise bitmaps will be loaded into system RAM (rather than video RAM)
    // and be really slow
    display = al_create_display(CANVAS_WIDTH, CANVAS_HEIGHT);

    *event_queue = al_create_event_queue();
    // TODO free this timer?
    *timer = al_create_timer(1/60.0);
    if (!(*timer))
        die("couldn't create timer\n");
    al_start_timer(*timer);

    al_register_event_source(*event_queue, al_get_display_event_source(display));
    al_register_event_source(*event_queue, al_get_timer_event_source(*timer));
    al_register_event_source(*event_queue, al_get_mouse_event_source());

    return display;
}

void free_allegro(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_TIMER *timer)
{
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue); 
    al_destroy_display(display);
}

int main(void)
{
    // stuff for init_allegro
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *timer;
    ALLEGRO_DISPLAY *display;

    // stuff for init_audio
    PaStream *audio_stream;
    float audio_level = 0;

    // stuff for load_resources
    ALLEGRO_FONT *font = NULL; // so clang is ok it being passed to load_resources
    intro_resource_struct intro_resources;
    object objects[OBJECTS_END];

    game_state_struct game_state;

    // initialisation
    {
        srand(time(NULL));
        display = init_allegro(&event_queue, &timer);
        audio_stream = init_portaudio(&audio_level);
        load_resources(&font, &intro_resources, objects);
        reset_objects(objects);
        game_state.scene = TITLE;
    }

    // game loop
    while (1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            handle_click(&(game_state.scene));
        else if (ev.type == ALLEGRO_EVENT_TIMER)    // time for next frame to be drawn
            tick(&game_state, audio_level, objects, display, font, &intro_resources);
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || game_state.scene == QUIT)
            break;
    }

    free_resources(font, &intro_resources, objects);
    free_portaudio(audio_stream);
    free_allegro(display, event_queue, timer);

    return 0;
}