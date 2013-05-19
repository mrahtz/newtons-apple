#include "audio.h"

static int check_pa_err(char *where, PaError err)
{
    if (err != paNoError) {
        fprintf(stderr, "audio error at %s: %s\n", where, Pa_GetErrorText(err)); // TODO better errors
        //exit(1);
        return 1;
    }
    return 0;
}

enum testmode_enum
{
    NOTEST,
    SINUSOID_SLOW,
    SINUSOID_SPREAD,
    MIN,
    MAX,
    ZERO
};
static const enum testmode_enum TESTMODE = NOTEST;

static void write_test_buffer(float *buffer, unsigned long framesPerBuffer, float t)
{
    int i;
    float multiplier;
    float t_mod_10;
    const float pi = 3.14;

    for (i = 0; i < framesPerBuffer; i++) {
        switch(TESTMODE) {
            case SINUSOID_SLOW:
                /* 1/8 Hz */
                buffer[i] = 0.04*sin((1.0/8)*2*pi*t);
                break;
            case SINUSOID_SPREAD:
                // vary from 0 hz to 10 kHz over 10 seconds
                /* can't actually do mod because of floats */
                t_mod_10 = t - 10*floor(t/10);
                /* ranges from 1 Hz to ~ 9 kHz */
                multiplier = pow(2.5, t_mod_10);
                buffer[i] = 0.031*sin(multiplier*2*pi*t);
                break;
            case MIN:               // float from portaudio ranges from -1.0 ...
                buffer[i] = -1.0;
                break;
            case MAX:
                buffer[i] = +1.0;   // to +1.0
                break;
            case ZERO:
                buffer[i] = 0;
                break;
            case NOTEST:    // just in case :)
                break;
        }
    }
}

// stores average amplitude of buffer in userData pointer
static int record_callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData)
{
    int i;
    float *in = (float *) inputBuffer;
    float *mean = (float *) userData;
    float sum = 0;

    if (TESTMODE != NOTEST) {
        // currentTime is a double typedef'd to a PaTime
        write_test_buffer(in, framesPerBuffer,
                          (float) timeInfo->currentTime); 
    }

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
