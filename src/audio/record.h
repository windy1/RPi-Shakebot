#ifndef SHAKESPEARE_RECORD_H
#define SHAKESPEARE_RECORD_H

#include <portaudio.h>

#define MAX_SECONDS         5
#define NUM_CHANNELS        2
#define SAMPLE_RATE         44100
#define FRAMES_PER_BUFFER   512
#define SAMPLE_SILENCE      0
#define SAMPLE_FORMAT       paInt16

typedef uint16_t Sample;

namespace sb {

    struct AudioData {
        int     frameIndex;
        int     maxFrameIndex;
        Sample *recordedSamples;
    };

    typedef void RecordCallback(AudioData*);

    void playAudio(AudioData &data);

    bool startRecording(RecordCallback callback);

    bool stopRecording();

}

#endif //SHAKESPEARE_RECORD_H
