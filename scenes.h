#ifndef SCENES_H
#define SCENES_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <math.h>	// for pow() in show_intro
#include "globals.h"
#include "helpers.h"
#include "game.h"

void show_titlescreen(ALLEGRO_FONT *font, object *newton);
void init_game(object *objects, int *lives, int *score);
int show_intro(object *objects, ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *tree);
int show_instructions(object *objects, ALLEGRO_DISPLAY *display,
	ALLEGRO_BITMAP *instructions1, ALLEGRO_BITMAP *instructions2);
void draw_game(const object *objects,
               ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font,
               const int score, const int lives);
int game_tick(object *objects, const float audio_level, int *lives, int *score);
void show_gameover(int score, ALLEGRO_FONT *font);

#endif