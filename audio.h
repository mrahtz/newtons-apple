#ifndef AUDIO_H
#define AUDIO_H

#include <portaudio.h>
#include <math.h> // for fabs() in record_callback
#include "globals.h"

void check_pa_err(char *where, PaError err);
int record_callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userdata);
PaStream * init_portaudio(float *userdata);
void free_portaudio(PaStream *audio_stream);

#endif