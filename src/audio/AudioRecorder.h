#ifndef SHAKESPEARE_AUDIORECORDER_H
#define SHAKESPEARE_AUDIORECORDER_H

#include "audio.h"
#include <portaudio.h>

using namespace std;

namespace sb {

    class AudioRecorder {

        AudioData           data;
        PaStream            *stream         = NULL;
        PaStreamParameters  inputParams;
        int                 sampleRate      = SAMPLE_RATE;
        int                 channelCount    = NUM_CHANNELS;
        int                 deviceIndex     = DEVICE_INDEX;
        bool                initialized     = false;

        bool terminate();

        bool abort();

    public:

        ~AudioRecorder();

        bool init();

        operator bool();

        bool isActive();

        int getSampleRate() const;

        void setSampleRate(int sampleRate);

        int getChannelCount() const;

        void setChannelCount(int channelCount);

        int getDeviceIndex() const;

        bool setDeviceIndex(int deviceIndex);

        bool record(int seconds);

    };

}

#endif //SHAKESPEARE_AUDIORECORDER_H
