#include <stdlib.h>
#include <portmidi.h>
#include <porttime.h>
#include <stdio.h>


#define MD_NOTEON 0x90
#define MD_NOTEOFF 0x80
#define MD_PRG  0xC0
#define SBYTE(mess,chan) mess | chan

PmError retval;
PortMidiStream *mstream;

int random_number(int lower_range, int upper_range);
void pause(int time_in_milliseconds);
void midi_note(int pitch, int channel, int velocity);
void program_change(int channel, int instrument);
void midi_start(void);
void midi_close(void);


int random_number(int lower_range, int upper_range)
{
    int x;

    if (upper_range < lower_range)
    {
        x = upper_range;
        upper_range = lower_range;
        lower_range = x;
    }
    return lower_range + rand() %(upper_range -lower_range +1);

}

void pause(int time_in_milliseconds)
{
    long time;

    time = Pt_Time(NULL);
    while(Pt_Time(NULL) - time < time_in_milliseconds);

}

void midi_note(int pitch, int channel, int velocity)
{
    int channel1,  velocity1;

    channel1 = (channel - 1) % 16;
    velocity1 = velocity % 128;

    Pm_WriteShort(mstream, 0, Pm_Message(SBYTE(MD_NOTEON,channel1), pitch, velocity1));
}

void program_change(int channel, int instrument)
{
    int channel1, instrument1;

    instrument1 = (instrument -1) % 128;
    channel1 = (channel - 1) % 16;

    Pm_WriteShort(mstream, 0, Pm_Message(SBYTE(MD_PRG,channel1), instrument1, 0));
}

void midi_start(void)
{
    Pm_Initialize();
    Pt_Start(1, NULL, NULL);
    retval = Pm_OpenOutput(&mstream, 0,NULL,512,NULL,NULL,0);
    if(retval != pmNoError)
    {
        printf("error: %s \n", Pm_GetErrorText(retval));
    }
    else /* set channel 1 to grand piano */
        program_change(1, 1);
}

void midi_close(void)
{
    Pm_Close(mstream);
    Pm_Terminate();
}

