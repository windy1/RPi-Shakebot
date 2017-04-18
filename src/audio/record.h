#ifndef SHAKESPEARE_RECORD_H
#define SHAKESPEARE_RECORD_H

#include <portaudio.h>
#include "audio.h"

namespace sb {

    typedef void RecordCallback(AudioData*);

    /**
     * Plays the specified AudioData to the default output device.
     *
     * @param data Data to play
     */
    bool playAudio(AudioData &data);

    /**
     * Starts recording from the default input device.
     *
     * @param callback Completion callback
     * @return True if successful
     */
    bool startRecording(RecordCallback *callback);

    /**
     * Stops recording audio.
     *
     * @return True if successful
     */
    bool stopRecording();

}

#endif //SHAKESPEARE_RECORD_H
