#ifndef SHAKESPEARE_RECORD_H
#define SHAKESPEARE_RECORD_H

#include <portaudio.h>

typedef float Sample;

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
