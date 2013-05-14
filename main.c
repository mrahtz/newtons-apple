#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <portaudio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"
#include "utilities.h"
#include "resources.h"
#include "audio.h"
#include "tick.h"

// defined in images/images.c, which is included
// by resources.c
extern unsigned char newtonhead_png[];
extern int newtonhead_png_size;

ALLEGRO_DISPLAY * init_allegro(ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_TIMER **frame_timer)
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
    // TODO free this frame_timer?
    *frame_timer = al_create_timer(1/60.0);
    if (!(*frame_timer))
        die("couldn't create frame_timer\n");
    al_start_timer(*frame_timer);

    al_register_event_source(*event_queue, al_get_display_event_source(display));
    al_register_event_source(*event_queue, al_get_timer_event_source(*frame_timer));
    al_register_event_source(*event_queue, al_get_mouse_event_source());

    return display;
}

void free_allegro(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_TIMER *frame_timer)
{
    al_destroy_timer(frame_timer);
    al_destroy_event_queue(event_queue); 
    al_destroy_display(display);
}

/*void show_error_dialog(ALLEGRO_DISPLAY *display, char *text)
{
    al_init_native_dialog_addon();
    char *title = "Error";
    char *heading = "Failed to grab microphone";
    char *buttons = "Quit";
    al_show_native_message_box(display, title, heading, text, buttons, ALLEGRO_MESS
    int al_show_native_message_box(ALLEGRO_DISPLAY *display,
    char const *title, char const *heading, char const *text,
    char const *buttons, int flags)
    al_shutdown_native_dialog_addon();
}*/

int main(void)
{
    // stuff for init_allegro
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *frame_timer;
    ALLEGRO_DISPLAY *display;

    // stuff for init_audio
    PaStream *audio_stream;
    float audio_level = 0;

    // stuff for load_resources
    ALLEGRO_FONT *font = NULL; // so clang is ok it being passed to load_resources
    intro_resource_struct intro_resources;
    object objects[OBJECTS_END];

    // just used in main
    ALLEGRO_BITMAP *head_bitmap;

    game_state_struct game_state;

    // initialisation
    {
        srand(time(NULL));
        display = init_allegro(&event_queue, &frame_timer);
        al_set_window_title(display, "Newton's Apple");

        head_bitmap = load_packed_bitmap(newtonhead_png, newtonhead_png_size);
        al_set_display_icon(display, head_bitmap);

        audio_stream = init_portaudio(&audio_level);
        load_resources(&font, &intro_resources, objects);

        //game_state.scene = INIT_TITLE;
        game_state.scene = INIT_GAME_WITH_RESET;
        objects[APPLE].reset_x_vel = 18;
        game_state.anim_state.last_frame_n = 0;
        game_state.anim_state.frame_n = 0;
        game_state.anim_state.sprite_n = 1;
        game_state.anim_state.velocity = 0;
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

    al_destroy_bitmap(head_bitmap);
    free_resources(font, &intro_resources, objects);
    free_portaudio(audio_stream);
    free_allegro(display, event_queue, frame_timer);

    return 0;
}
