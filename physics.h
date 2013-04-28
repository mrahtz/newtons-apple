#ifndef PHYSICS_H
#define PHYSICS_H

#include "globals.h"
#include "graphics.h"
#include "utilities.h"

int simulate_objects(object *objects, const float audio_level);
int check_collision(const object *o1, const object *o2);
int check_ground_collision(object *o, object *ground);
void update_physics(object *objects, int n, int mode);
void reset_object_position(object *objects, int object_n);
void reset_object_physics(object *objects, int object_n);

#endif