#ifndef SHAKESPEARE_AUDIORECORDER_H
#define SHAKESPEARE_AUDIORECORDER_H

#include "audio.h"
#include "../sb.h"

namespace sb {

    /**
     * Function type to receive recorded audio data.
     */
    typedef std::function<void()> CaptureCallback;

    /**
     * A client to handle audio capture and playback.
     */
    class AudioClient : public NonAssignable {

        AudioData           dat;                        /// where the samples will be recorded to
        AudioDevice         captureDevice;              /// the device to capture samples
        CaptureCallback     captureCallback;            /// callback to call once recording is complete
        AudioDevice         playbackDevice;             /// the device to playback audio data to
        PaStream            *stream         = NULL;     /// the audio input/output stream
        bool                initialized     = false;    /// initialized state

        /// returns true if a new input or output audio stream can be opened
        bool canOpenStream();

    public:

        ~AudioClient();

        /**
         * Initializes the client and PortAudio.
         *
         * @return True if initialized
         */
        bool init();

        /**
         * Returns a pointer to the client's captured data.
         *
         * @return Pointer to captured data
         */
        const AudioData* data() const;

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
         * Returns the callback to be called once recording has been completed.
         *
         * @return Callback function
         */
        CaptureCallback getCaptureCallback() const;

        /**
         * Sets the callback to be called once recording has been completed.
         * Note: The stream should be altered from the callback function / PA
         * thread.
         *
         * @param captureCallback Callback function
         */
        void setCaptureCallback(CaptureCallback captureCallback);

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
        bool record(int seconds);

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
         * @param dat Data to play
         * @return True if successful
         */
        bool play();

    };

}

#endif //SHAKESPEARE_AUDIORECORDER_H
