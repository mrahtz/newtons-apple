#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "utilities.h"
#include "globals.h"
#include "physics.h"

void init_sprite(object *o, int object_n);
void reset_object_physics(object *o, int object_n);
void reset_object_position(object *objects, int object_n);
int check_if_offscreen(const object *o);
int check_collision(const object *o1, const object *o2);
int simulate_objects(object *objects, float audio_level);
void draw_objects_with_animate(const object *objects, int animate_time);
void draw_object_sprite_n(const object *o, int sprite_n);
void rotate_ground(ALLEGRO_BITMAP *ground, ALLEGRO_DISPLAY *display, int amount);
void update_physics(object *objects, int n, int mode);

#endif
