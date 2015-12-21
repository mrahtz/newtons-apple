#ifndef AUDIO_H
#define AUDIO_H

#include <portaudio.h>
#include <math.h> /* for fabs() in record_callback and sin() in test functions */
#include <stdio.h>
#include "globals.h"

PaStream * init_portaudio(float *userdata);
void free_portaudio(PaStream *audio_stream);

#endif
