#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "globals.h"
#include "utilities.h"

void rotate_ground(ALLEGRO_BITMAP *ground, ALLEGRO_DISPLAY *display, int amount);
void draw_objects_with_animate(const object *objects, int animate_time);
void draw_object_sprite_n(const object *o, int sprite_n);
int check_if_offscreen(const object *o);
void init_sprite(object *o, int object_n);

#endif