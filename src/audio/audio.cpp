#include "AudioClient.h"

namespace sb {

    AudioClient audio;

    ostream &operator<<(ostream &out, const AudioDevice &device) {
        out << "AudioDevice{";
        out << "index=" << device.params.device << ", ";
        out << "channelCount=" << device.params.channelCount << ", ";
        out << "sampleFormat=" << typeid(Sample).name() << sizeof(Sample) * 8 << ", ";
        out << "suggestedLatency=" << device.params.suggestedLatency << "}";
        return out;
    }

    AudioClient* initAudio() {
        cout << "Initializing audio..." << endl;
        if (!audio.init()) {
            cerr << "Could not initialize client" << endl;
            return NULL;
        }
        if (!audio.setCaptureDevice(DEVICE_INDEX)) {
            cerr << "Could not initialize capture device" << endl;
            return NULL;
        }

        AudioDevice *device = audio.getCaptureDevice();
        device->params.channelCount = CHANNEL_COUNT_CAPTURE;
        device->params.sampleFormat = SAMPLE_FORMAT;
        device->bufferSize = BUFFER_SIZE_CAPTURE;
        cout << "- " << *device << endl;
        return &audio;
    }

}
