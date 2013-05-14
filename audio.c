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

enum testmode_enum
{
    NOTEST,
    SINUSOID_5KHZ,
    SINUSOID_SPREAD,
    MIN,
    MAX,
    ZERO
};
const enum testmode_enum testmode = MAX;

void write_test_buffer(float *buffer, unsigned long framesPerBuffer)
{
    int i;
    static float t = 0;
    float multiplier;

    for (i = 0; i < framesPerBuffer; i++) {
        switch(testmode) {
            case SINUSOID_5KHZ:
                buffer[i] = sin(5000 * t);
                break;
            case SINUSOID_SPREAD:
                // vary from 0 hz to 10 kHz over 10 seconds
                // frequency is t (in seconds) kHz
                multiplier = t/SAMPLERATE;
                buffer[i] = sin(multiplier*1000 * t);
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
        t += 1.0/SAMPLERATE;
    }
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

    if (testmode != NOTEST)
        write_test_buffer(in, framesPerBuffer);

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
                                SAMPLERATE,
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
