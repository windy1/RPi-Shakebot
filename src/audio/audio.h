#ifndef SHAKESPEARE_AUDIO_H_H
#define SHAKESPEARE_AUDIO_H_H

#include <cstdint>
#include <portaudio.h>
#include <iostream>

#define MAX_SECONDS             5           // seconds to record
#define CHANNEL_COUNT_CAPTURE   1           // hw limited to mono
#define CHANNEL_COUNT_PLAYBACK  2           // playback must be stereo
#define SAMPLE_RATE             48000       // optimal is 16000, limited by hw
#define BUFFER_SIZE_CAPTURE     24000       // frames to capture each cycle
#define BUFFER_SIZE_PLAYBACK    16384       // frames to playback each cycle
#define SAMPLE_SILENCE          0           // silence sample value
#define SAMPLE_FORMAT           paInt16     // hw requires 16-bit samples
#define ENCODING                "LINEAR16"  // the encoding id for voice api
#define LANGUAGE_CODE           "en-US"     // language being recorded
#ifdef __APPLE__                            //
#define DEVICE_INDEX            -1          // use default device
#else                                       //
#define DEVICE_INDEX             2          // hardcoded for RPi
#endif

typedef int16_t Sample;

using namespace std;

/**
 * Raw PCM audio data. Each sample recorded represents an amplitude of the
 * sound wave received.
 */
struct AudioData {
    int     frameIndex;                 // incremented iterating sample data
    int     numFrames;                  // the total amount of frames in the recording
    Sample  *recordedSamples;           // block of memory containing samples
};

struct AudioDevice {

    PaStreamParameters  params;
    unsigned long       bufferSize      = 0;
    int                 sampleRate      = SAMPLE_RATE;

    friend ostream& operator<<(ostream &out, const AudioDevice &device);

};

#endif //SHAKESPEARE_AUDIO_H_H
