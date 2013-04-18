#ifndef GAME_H
#define GAME_H

#include "globals.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

void init_object(object *o, int object_n);
void reset_object_physics(object *o, int object_n);
int check_if_offscreen(object *o);
int check_collision(object *o1, object *o2);
int simulate_world(object *objects, float audio_level);
void draw_world(object *objects, ALLEGRO_BITMAP *ground, int animate_timer);
void rotate_ground(ALLEGRO_BITMAP *ground, ALLEGRO_DISPLAY *display, int amount);

#endif
