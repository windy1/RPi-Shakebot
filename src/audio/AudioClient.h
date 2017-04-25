#ifndef SHAKESPEARE_AUDIORECORDER_H
#define SHAKESPEARE_AUDIORECORDER_H

#include "audio.h"
#include <portaudio.h>

using namespace std;

namespace sb {

    class AudioClient {

        AudioData           data;
        AudioDevice         captureDevice;
        AudioDevice         playbackDevice;
        PaStream            *stream         = NULL;
        bool                initialized     = false;

        bool abort();

    public:

        ~AudioClient();

        bool init();

        bool setCaptureDevice(int index);

        bool setDefaultCaptureDevice();

        AudioDevice* getCaptureDevice();

        bool isActive();

        bool canRecord();

        bool record(int seconds);

    };

}

#endif //SHAKESPEARE_AUDIORECORDER_H
