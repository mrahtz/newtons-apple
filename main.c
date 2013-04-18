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

void show_titlescreen(ALLEGRO_FONT *font, object *newton)
{
    const int min_x = CANVAS_WIDTH * 1.0/4;
    const int max_x = CANVAS_WIDTH * 3.0/4;
    static int x = min_x;
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
                 "Tap to play!");

    if (animate_timer % 5 == 0)
        sprite_n = !sprite_n;
    if (sprite_n == 0)
        sprite = newton->sprite1;
    else
        sprite = newton->sprite2;
    al_draw_bitmap(sprite,
                   x, CANVAS_HEIGHT * 3.0/4,
                   0);

    animate_timer++;
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
    object objects[LAST_OBJECT];
    int animate_timer = 0;
    int score = 0;
    int lives = 1;
    int scene = TITLE;
    int font_line_height;

    // initialisation
    {
        enum object_ctr i;

        al_init();
        al_init_image_addon();
        al_init_font_addon();
        al_init_ttf_addon();
        al_install_mouse();
        for (i = 0; i < LAST_OBJECT; i++)
            init_object(&objects[i], i);
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

        al_register_event_source(event_queue, al_get_mouse_event_source());

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
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (scene == TITLE) {
                show_titlescreen(font, &objects[3]);
            } else if (scene == GAME) {   // main game
                if (simulate_objects(objects, audio_level))
                    lives--;
                if (lives == 0) {
                    scene = GAMEOVER;
                    continue;
                }
                rotate_ground(ground, display, objects[APPLE].x_vel);
                // TODO remove the offset?
                draw_objects(objects, 0, animate_timer);
                al_draw_bitmap(ground, 0, CANVAS_HEIGHT-al_get_bitmap_height(ground), 0);
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
            } else if (scene == GAMEOVER)
                show_gameover(score, font);
            
            al_flip_display();
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && scene == GAMEOVER) {
            ALLEGRO_MOUSE_STATE m_state;
            al_get_mouse_state(&m_state);
            int x = m_state.x;

            if (x < CANVAS_WIDTH/2.0) {
                // TODO: reset physics
                lives = 3;
                score = 0;
                scene = GAME;
            } else
                break;
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && scene == TITLE) {
            scene = GAME;
        }
    }
    
    free_resources(display, objects, event_queue, audio_stream);

    return 0;
}
