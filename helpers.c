#include "helpers.h"
#include "globals.h"

void check_pa_err(char *where, PaError err)
{
    if (err != paNoError) {
        fprintf(stderr, "audio error at %s: %s\n", where, Pa_GetErrorText(err));
        exit(1);
    }
}


PaStream * portaudio_init(PaStreamCallback *callback, float *data)
{
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    check_pa_err("init", err);
    err = Pa_OpenDefaultStream( &stream,
                                1,          // 1 input channel
                                0,          // no output channels
                                paFloat32,     // sample format
                                44100,      // sample rate
                                256,        // frames per buffer
                                callback,
                                data );
    check_pa_err("stream open", err);
    err = Pa_StartStream(stream);
    check_pa_err("stream start", err);
    
    return stream;
}

float rand_between(int min, int max)
{
    int range = max-min;
    float r = (float) rand();
    return min + range * r/RAND_MAX;
}

// TODO: this doesn't clean stuff up!
void die(char *message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    exit(1);
}

void free_resources(ALLEGRO_DISPLAY *display, object *objects, ALLEGRO_EVENT_QUEUE *event_queue, PaStream *audio_stream)
{
    enum object_ctr i;
    // allegro stuff
    al_destroy_display(display);
    for (i = 0; i < LAST_OBJECT; i++) {
        al_destroy_bitmap(objects[i].sprite1);
        al_destroy_bitmap(objects[i].sprite2);
    }
    al_destroy_event_queue(event_queue);

    // portaudio stuff
    Pa_StopStream(audio_stream);
    Pa_Terminate();
}
