#ifndef RESOURCES_H
#define RESOURCES_H

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_memfile.h>

#include "globals.h"
#include "utilities.h"
#include "graphics.h"
#include "object_init.h"

ALLEGRO_FONT * load_packed_font(unsigned char *mem, int mem_size, int font_size);
ALLEGRO_BITMAP * load_packed_bitmap(unsigned char *mem, int size);
void load_resources(ALLEGRO_FONT **font, intro_resource_struct *intro_resources, object *objects);
void free_resources(ALLEGRO_FONT *font, intro_resource_struct *intro_resources, object *objects);

#endif
