#ifndef RESOURCES_H
#define RESOURCES_H

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include "globals.h"
#include "utilities.h"
#include "graphics.h"

void load_resources(ALLEGRO_FONT **font, intro_resource_struct *intro_resources, object *objects);
void free_resources(ALLEGRO_FONT *font, intro_resource_struct *intro_resources, object *objects);

#endif
