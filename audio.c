#include "audio.h"

int check_pa_err(char *where, PaError err)
{
    if (err != paNoError) {
        fprintf(stderr, "audio error at %s: %s\n", where, Pa_GetErrorText(err)); // TODO better errors
        //exit(1);
        return 1;
    }
    return 0;
}

// stores average amplitude of buffer in userData pointer
int record_callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData)
{
    int i;
    float *in = (float *) inputBuffer;
    float *mean = (float *) userData;
    float sum = 0;

    for (i = 0; i < framesPerBuffer; i++) {
        sum += fabs(in[0]);
    }
    *mean = sum/framesPerBuffer;

    return 0;
}

PaStream * init_portaudio(float *userdata)
{
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if (check_pa_err("init", err))
        return NULL;
    err = Pa_OpenDefaultStream( &stream,
                                1,          // 1 input channel
                                0,          // no output channels
                                paFloat32,  // sample format
                                44100,      // sample rate
                                256,        // frames per buffer
                                record_callback,
                                userdata );
    if (check_pa_err("stream open", err))
        return NULL;
    err = Pa_StartStream(stream);
    if (check_pa_err("stream start", err))
        return NULL;
    
    return stream;
}

void free_portaudio(PaStream *audio_stream)
{
    // TODO check for error?
    Pa_StopStream(audio_stream);
    Pa_Terminate();
}