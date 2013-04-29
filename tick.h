#ifndef TICK_H
#define TICK_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include "globals.h"
#include "scenes.h"

void handle_click(int *scene);
void tick(game_state_struct *state, float audio_level, object *objects,
          ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, intro_resource_struct *intro_resources);

#endif