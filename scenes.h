#ifndef SCENES_H
#define SCENES_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include "globals.h"
#include "helpers.h"
#include "game.h"

void show_titlescreen(ALLEGRO_FONT *font, object *newton);
void show_titlescreen(ALLEGRO_FONT *font, object *newton);
void draw_game(const object *objects,
               ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *ground, ALLEGRO_FONT *font,
               const int score, const int lives);
int game_tick(object *objects, const float audio_level, int *lives, int *score);
void init_game(object *objects, int *lives, int *score);
void init_game(object *objects, int *lives, int *score);
int game_tick(object *objects, const float audio_level, int *lives, int *score);
void draw_game(const object *objects,
               ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *ground, ALLEGRO_FONT *font,
               const int score, const int lives);
void show_gameover(int score, ALLEGRO_FONT *font);

#endif