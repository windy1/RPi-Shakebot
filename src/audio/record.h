#ifndef SHAKESPEARE_RECORD_H
#define SHAKESPEARE_RECORD_H

#include <portaudio.h>

#define MAX_SECONDS         5
#define NUM_CHANNELS        2
#define SAMPLE_RATE         44100
#define FRAMES_PER_BUFFER   512
#define SAMPLE_SILENCE      0
#define SAMPLE_FORMAT       /*paInt16*/ paInt8

#define ENCODING            "LINEAR16"
#define LANGUAGE_CODE       "en-US"

typedef /*uint16_t*/ uint8_t Sample;

namespace sb {

    /**
     * Represents, raw, recorded, PCM, audio data.
     */
    struct AudioData {
        int     frameIndex;
        int     maxFrameIndex;
        Sample *recordedSamples;
    };

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
