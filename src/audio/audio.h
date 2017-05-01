#ifndef SHAKESPEARE_AUDIO_H_H
#define SHAKESPEARE_AUDIO_H_H

#include <portaudio.h>
#include <festival/festival.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>

#define MAX_SECONDS             5           /// seconds to record
#define CHANNEL_COUNT_CAPTURE   1           /// hw limited to mono
#define CHANNEL_COUNT_PLAYBACK  2           /// playback must be stereo
#define SAMPLE_RATE             48000       /// optimal is 16000, limited by hw
#define BUFFER_SIZE_CAPTURE     24000       /// frames to capture each cycle
#define BUFFER_SIZE_PLAYBACK    16384       /// frames to playback each cycle
#define SAMPLE_SILENCE          0           /// silence sample value
#define SAMPLE_FORMAT           paInt16     /// hw requires 16-bit samples
#define ENCODING                "LINEAR16"  /// the encoding id for voice api
#define LANGUAGE_CODE           "en-US"     /// language being recorded
#ifdef __APPLE__                            ///
#define DEVICE_INDEX            -1          /// macOS -> use default device
#else                                       ///
#define DEVICE_INDEX             2          /// RPi -> use hardcoded device
#endif

using namespace std;

namespace sb {

    /// Sample data type
    typedef int16_t Sample;

    class AudioClient;

    /**
     * Raw PCM audio data. Each sample recorded represents an amplitude of the
     * sound wave received.
     */
    struct AudioData {

        int         frameIndex;                     /// incremented iterating sample data
        int         frameCount;                     /// the total amount of frames in the recording
        int         captureChannels;                /// the amount of channels the data was sampled with
        Sample      *recordedSamples    = NULL;     /// block of memory containing samples
        AudioClient *client             = NULL;     /// reference to client

        AudioData();

        AudioData(const AudioData &data);

        ~AudioData();

        /**
         * Returns the total amount of samples allocated for this data.
         *
         * @return Total samples
         */
        int sampleCount() const;

        /**
         * Returns the total amount of bytes allocated to this data.
         *
         * @return Bytes allocated for data
         */
        unsigned size() const;

    };

    /**
     * An audio input or output device.
     */
    struct AudioDevice {

        PaStreamParameters  params;                     /// PA parameters
        unsigned long       bufferSize = 0;             /// amount of samples to process per period
        int                 sampleRate = SAMPLE_RATE;   /// sample rates in Hz

        friend ostream &operator<<(ostream &out, const AudioDevice &device);

    };

    /**
     * Initializes a new audio client for the application.
     *
     * @return Pointer to new audio client
     */
    AudioClient* initAudio();

}

#endif //SHAKESPEARE_AUDIO_H_H
