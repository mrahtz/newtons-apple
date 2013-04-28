#include "globals.h"

int check_collision(const object *o1, const object *o2);
int check_ground_collision(object *o, object *ground);
void update_physics(object *objects, int n, int mode);
