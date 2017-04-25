#include "audio.h"

ostream& operator<<(ostream &out, const AudioDevice &device) {
    out << "AudioDevice{";
    out << "index=" << device.params.device << ", ";
    out << "channelCount=" << device.params.channelCount << ", ";
    out << "sampleFormat=" << typeid(Sample).name() << sizeof(Sample) * 8 << ", ";
    out << "suggestedLatency=" << device.params.suggestedLatency << "}";
    return out;
}
