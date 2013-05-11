#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "globals.h"
#include "utilities.h"

void rotate_ground(object *ground, ALLEGRO_DISPLAY *display, int amount);
void draw_objects_with_animate(object *objects, animation_state_struct *state);
void draw_object_sprite_n(object *o, int sprite_n);
int check_if_offscreen(object *o);
void draw_game(object *objects,
               ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font,
               game_state_struct *game_state);

#endif