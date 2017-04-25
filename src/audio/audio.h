#ifndef SHAKESPEARE_AUDIO_H_H
#define SHAKESPEARE_AUDIO_H_H

#include <cstdint>

#define MAX_SECONDS         5           // seconds to record
#define NUM_CHANNELS        1           // hw limited to mono
#define SAMPLE_RATE         44800       // optimal is 16000, limited by hw
#define FRAMES_PER_BUFFER   512         // frames to load into memory each cycle
#define SAMPLE_SILENCE      0           // silence sample value
#define SAMPLE_FORMAT       paInt16     // hw requires 16-bit samples
#define ENCODING            "LINEAR16"  // the encoding id for voice api
#define LANGUAGE_CODE       "en-US"     // language being recorded
#ifdef __APPLE__                        //
#define DEVICE_INDEX -1                 // use default device
#else                                   //
#define DEVICE_INDEX 2                  // hardcoded for RPi
#endif

typedef uint16_t Sample;

/**
 * Raw PCM audio data. Each sample recorded represents an amplitude of the
 * sound wave received.
 */
struct AudioData {
    int     frameIndex;                 // incremented iterating sample data
    int     numFrames;                  // the total amount of frames in the recording
    Sample  *recordedSamples;           // block of memory containing samples
};

#endif //SHAKESPEARE_AUDIO_H_H
