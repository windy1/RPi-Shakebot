#ifndef SHAKESPEARE_AUDIODEVICE_H
#define SHAKESPEARE_AUDIODEVICE_H

#include "audio.h"
#include <iostream>

namespace sb {

    struct AudioDevice {

        PaStreamParameters  params;
        unsigned long       bufferSize      = 0;
        int                 sampleRate      = SAMPLE_RATE;

        friend ostream& operator<<(ostream &out, const AudioDevice &device);

    };

}

#endif //SHAKESPEARE_AUDIODEVICE_H
