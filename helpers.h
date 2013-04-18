#ifndef HELPERS_H
#define HELPERS_H

#include "globals.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <portaudio.h>
#include <stdlib.h> // for exit() in die()

void check_pa_err(char *where, PaError err);
PaStream * portaudio_init(PaStreamCallback *callback, float *data);
float rand_between(int min, int max);
void die(char *message, ...);
void free_resources(ALLEGRO_DISPLAY *display, object *objects, ALLEGRO_EVENT_QUEUE *event_queue, PaStream *audio_stream);

#endif
