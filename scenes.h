#ifndef SCENES_H
#define SCENES_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <math.h>	// for pow() in show_intro
#include "globals.h"
#include "graphics.h"
#include "physics.h"

void show_titlescreen(ALLEGRO_FONT *font, object *newton);
void init_game(game_state_struct *game_state, object *objects);
void reset_objects(object *objects);
int show_intro(object *objects, ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *tree, ALLEGRO_FONT *font);
int show_instructions(object *objects, ALLEGRO_DISPLAY *display,
	ALLEGRO_BITMAP *instructions1, ALLEGRO_BITMAP *instructions2);
int game_tick(object *objects, float audio_level, int *lives, int *score);
void show_gameover(int score, ALLEGRO_FONT *font);

#endif