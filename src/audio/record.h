#ifndef SHAKESPEARE_RECORD_H
#define SHAKESPEARE_RECORD_H

#include <functional>
#include "audio.h"

namespace sb {

    /**
     * Function type to receive recorded audio data.
     */
    typedef std::function<void(const AudioData*)> RecordCallback;

    /**
     * Returns true if audio is currently being recorded.
     *
     * @return True if recording
     */
    bool isActive();

    /**
     * Starts recording from the default input device.
     *
     * @param callback Completion callback
     * @return True if successful
     */
    bool startRecording(RecordCallback callback);

    /**
     * Stops recording audio.
     */
    bool stopRecording();

    /**
     * Plays the specified AudioData to the default output device.
     *
     * @param data Data to play
     */
    bool playAudio(const AudioData *data);

}

#endif //SHAKESPEARE_RECORD_H
