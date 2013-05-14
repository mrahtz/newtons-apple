#ifndef OBJECT_INIT_H
#define OBJECT_INIT_H

#include "globals.h"
#include "utilities.h"

void load_respawn(object *o, int n, float multiplier);
void reset_objects(object *objects);
void reset_object_physics(object *objects, int object_n);

#endif