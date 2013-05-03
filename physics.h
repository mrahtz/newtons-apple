#ifndef PHYSICS_H
#define PHYSICS_H

#include "globals.h"
#include "graphics.h"
#include "utilities.h"
#include "object_init.h"

int simulate_objects(object *objects, float audio_level);
int check_collision(object *o1, object *o2);
int check_ground_collision(object *o, object *ground);
void update_physics(object *objects, int n, int mode);

#endif