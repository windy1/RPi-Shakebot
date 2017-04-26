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

        bool abortRecord();

        bool canOpen();

        void resetData();

    public:

        ~AudioClient();

        /**
         * Initializes the client and PortAudio.
         *
         * @return True if initialized
         */
        bool init();

        /**
         * Resets and reinitializes the client and PortAudio.
         *
         * @return True if successful
         */
        bool reset();

        /**
         * Sets the device to use for audio capture. This method will use the
         * default device if -1 is used for the index. Returns true if
         * successful.
         *
         * @param index Device index
         * @return True if successful
         */
        bool setCaptureDevice(int index);

        /**
         * Sets the capture device as the default capture device. Returns true
         * if successful.
         *
         * @return True if successful
         */
        bool setDefaultCaptureDevice();

        /**
         * Returns the capture device to use.
         *
         * @return Capture device
         */
        AudioDevice* getCaptureDevice();

        /**
         * Returns true if this client currently has a stream active.
         *
         * @return True if opened
         */
        bool isStreamActive();

        /**
         * Returns true if the stream is stopped.
         *
         * @return True if stopped
         */
        bool isStreamStopped();

        /**
         * Attempts to close the active stream.
         *
         * @return True if closed
         */
        bool close();

        /**
         * Returns true if the client is ready to record.
         *
         * @return True if ready to record
         */
        bool canRecord();

        /**
         * Starts a new asynchronous recording for the specified amount of
         * seconds and calls the specified callback with audio data on
         * completion.
         *
         * @param seconds Seconds to record
         * @param callback Callback to call
         * @return True if successful
         */
        bool record(int seconds, RecordCallback callback);

        /**
         * Sets the device to use for audio playback. This method will use the
         * default device if -1 is used for the index. Returns true if
         * successful.
         *
         * @param index Device index
         * @return True if successful
         */
        bool setPlaybackDevice(int index);

        /**
         * Sets the capture device as the default playback device. Returns true
         * if successful.
         *
         * @return True if successful
         */
        bool setDefaultPlaybackDevice();

        /**
         * Returns the playback device to use.
         *
         * @return Playback device
         */
        AudioDevice* getPlaybackDevice();

        /**
         * Returns true if the client is ready to start playback.
         *
         * @return True if ready for playback
         */
        bool canPlay();

        /**
         * Starts playback on the specified audio data.
         *
         * @param data Data to play
         * @return True if successful
         */
        bool play(AudioData &data);

    };

}

#endif //SHAKESPEARE_AUDIORECORDER_H
