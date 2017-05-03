/*
 * RPi-Shakebot
 * ============
 * A voice recognition bot built for Prof. James Eddy's Computer Organization
 * (CS 121) class on the Raspberry Pi 3 Model B.
 *
 * References
 * ~~~~~~~~~~
 * [Festival]           : http://www.cstr.ed.ac.uk/projects/festival/manual/festival_28.html#SEC132
 * [PortAudio]          : http://www.portaudio.com/
 * [SFML]               : https://www.sfml-dev.org/
 * [libcurl]            : https://curl.haxx.se/libcurl/c/
 * [json]               : https://github.com/nlohmann/json
 * [Google Speech API]  : https://cloud.google.com/speech/docs/
 * [MediaWiki API]      : https://www.mediawiki.org/wiki/API:Main_page
 *
 * Copyright (C) Walker Crouse 2017 <wcrouse@uvm.edu>
 */
#include "AudioClient.h"
#if !__APPLE__
#include <string.h>
#endif

namespace sb {

    AudioClient audio;

    static bool initCaptureDevice();


    /* -- AudioData -- */

    AudioData::AudioData() {
    }

    AudioData::AudioData(const AudioData &data) {
        frameIndex = data.frameIndex;
        frameCount = data.frameCount;
        captureChannels = data.captureChannels;
        // copy sample memory block
        recordedSamples = (Sample*) malloc(data.size());
        memcpy(recordedSamples, data.recordedSamples, data.size());
    }

    AudioData::~AudioData() {
        if (recordedSamples != NULL) {
            free(recordedSamples);
        }
    }

    int AudioData::sampleCount() const {
        return frameCount * captureChannels;
    }

    unsigned AudioData::size() const {
        return sampleCount() * sizeof(Sample);
    }


    /* -- AudioDevice -- */

    ostream &operator<<(ostream &out, const AudioDevice &device) {
        out << "AudioDevice{";
        out << "index=" << device.params.device << ", ";
        out << "channelCount=" << device.params.channelCount << ", ";
        out << "sampleFormat=" << typeid(Sample).name() << sizeof(Sample) * 8 << ", ";
        out << "suggestedLatency=" << device.params.suggestedLatency << "}";
        return out;
    }


    /* -- Audio functions -- */

    AudioClient* initAudio() {
        cout << "Initializing audio..." << endl;
        if (!audio.init()) {
            cerr << "Could not initialize client" << endl;
            return NULL;
        }
        if (!initCaptureDevice()) {
            cerr << "Could not initialize capture device" << endl;
            return NULL;
        }
        return &audio;
    }

    bool initCaptureDevice() {
        if (!audio.setCaptureDevice(DEVICE_INDEX)) {
            return false;
        }
        AudioDevice *device = audio.getCaptureDevice();
        device->params.channelCount = CHANNEL_COUNT_CAPTURE;
        device->params.sampleFormat = SAMPLE_FORMAT;
        device->bufferSize = BUFFER_SIZE_CAPTURE;
        cout << "- " << *device << endl;
        return true;
    }

}
