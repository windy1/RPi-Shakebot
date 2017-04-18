#ifndef SHAKESPEARE_AUDIO_H_H
#define SHAKESPEARE_AUDIO_H_H

#include <cstdint>

#define MAX_SECONDS         5
#define NUM_CHANNELS        2
#define SAMPLE_RATE         44100
#define FRAMES_PER_BUFFER   512
#define SAMPLE_SILENCE      0
#define SAMPLE_FORMAT       /*paInt16*/ paInt8

#define ENCODING            "LINEAR16"
#define LANGUAGE_CODE       "en-US"

typedef /*uint16_t*/ uint8_t Sample;

/**
 * Represents, raw, recorded, PCM, audio data.
 */
struct AudioData {
    int     frameIndex;
    int     maxFrameIndex;
    Sample *recordedSamples;
};

#endif //SHAKESPEARE_AUDIO_H_H
