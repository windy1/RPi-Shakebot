#include "AudioClient.h"
#include <cassert>

namespace sb {

    /* Static C-Style callbacks */

    void paErr(PaError err) {
        cerr << "An error occurred while using the portaudio stream" << endl;
        cerr << "Error number: " << err << endl;
        cerr << "Error message: " << Pa_GetErrorText(err) << endl;
    }

    void printDevices() {
        // debug info
        cout << "Available Devices" << endl;
        for (int i = 0; i < Pa_GetDeviceCount(); i++) {
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
            cout << "- " << info->name << endl;
            cout << "    Max Input Channels: " << info->maxInputChannels << endl;
            cout << "    Max Output Channels: " << info->maxOutputChannels << endl;
            cout << "    Default Sample Rate: " << info->defaultSampleRate << endl;
        }
    }

    void onStreamFinished(void *audioData) {
        // called after a playback/capture stream is stopped
        // close stream and pass callback recorded data
        cout << "Stream complete." << endl;
        assert(audioData != NULL);
        AudioData *data = (AudioData*) audioData;
        data->callback(data);
        // free sample memory
        if (data->recordedSamples != NULL) {
            free(data->recordedSamples);
            data->recordedSamples = NULL;
        }
    }

    int captureSamples(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData) {

        // Called periodically while an input stream is active. It is important
        // that nothing particularly time consuming happens here.

        // unused
        (void) outputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        cout << "."; // TODO: remove

        AudioData       *data;                              // the data buffer we're writing to
        const Sample    *in;                                // the incoming samples
        Sample          *sampleData;                        // the slice of memory to write incoming data to
        long            frames;                             // amount of frames to process this period
        int             state;                              // the state of the stream (continue/complete)

        data = (AudioData*) userData;
        in = (const Sample*) inputBuffer;
        sampleData = &data->recordedSamples[data->frameIndex * CHANNEL_COUNT_CAPTURE];

        unsigned long framesLeft = (unsigned long) (data->numFrames - data->frameIndex);
        if (framesLeft < framesPerBuffer) {
            // write the rest of the frames
            frames = framesLeft;
            state = paComplete;
        } else {
            // write the standard amount of frames
            frames = framesPerBuffer;
            state = paContinue;
        }

        // take samples
        if (inputBuffer == NULL) {
            // no new data, sample silence
            for (long i = 0; i < frames; i++) {
                *sampleData++ = SAMPLE_SILENCE;
                if (CHANNEL_COUNT_CAPTURE == 2) {
                    *sampleData++ = SAMPLE_SILENCE;
                }
            }
        } else {
            for (long i = 0; i < frames; i++) {
                *sampleData++ = *in++;
                if (CHANNEL_COUNT_CAPTURE == 2) {
                    *sampleData++ = *in++;
                }
            }
        }

        data->frameIndex += frames;
        return state;
    }

    int playbackSamples(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                        void *userData) {
        // unused
        (void) inputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        AudioData   *data;                          // the sample data we are reading from
        Sample      *sampleData;                    // the slice of samples we will write this period
        Sample      *out = (Sample*) outputBuffer;  // the space to write sample to output device
        int         state;                          // the completion state of the stream

        data = (AudioData*) userData;
        sampleData = &data->recordedSamples[data->frameIndex * CHANNEL_COUNT_CAPTURE];

        // Note: The following has been hardcoded to write 1-channel input
        // (mono) to 2-channel output (stereo).

        unsigned int framesLeft = (unsigned int) (data->numFrames - data->frameIndex);
        if (framesLeft < framesPerBuffer) {
            int i = 0;
            for (i = 0; i < framesLeft; i++) {
                Sample sample = *sampleData++;
                //*out++ = *sampleData++;
                *out++ = sample;
                if (CHANNEL_COUNT_PLAYBACK == 2) {
                    //*out++ = *sampleData++;
                    *out++ = sample;
                }
            }
            for (; i < framesPerBuffer; i++) {
                *out++ = 0;
                if (CHANNEL_COUNT_PLAYBACK == 2) {
                    *out++ = 0;
                }
            }
            data->frameIndex += framesLeft;
            state = paComplete;
        } else {
            for (int i = 0; i < framesPerBuffer; i++) {
                Sample sample = *sampleData++;
                //*out++ = *sampleData++;
                *out++ = sample;
                if (CHANNEL_COUNT_PLAYBACK == 2) {
                    //*out++ = *sampleData++;
                    *out++ = sample;
                }
            }
            data->frameIndex += framesPerBuffer;
            state = paContinue;
        }

        return state;
    }

    PaError openInputStream(PaStream **stream, AudioDevice &captureDevice, AudioData &data) {
        return Pa_OpenStream(
                stream,                      //
                &captureDevice.params,       // input params
                NULL,                        // output params
                captureDevice.sampleRate,    //
                captureDevice.bufferSize,    //
                paClipOff,                   // disable clipping
                captureSamples,              // callback to read samples
                &data);                      // the data object to read into
    }

    PaError openOutputStream(PaStream **stream, AudioDevice &playbackDevice, AudioData &data) {
        return Pa_OpenStream(
                stream,
                NULL,
                &playbackDevice.params,
                playbackDevice.sampleRate,
                playbackDevice.bufferSize,
                paClipOff,
                playbackSamples,
                &data);
    }


    /* PortAudio wrapped in a more C++ friendly way */

    AudioClient::~AudioClient() {
        // cleanup
        if (isOpened()) {
            Pa_AbortStream(stream);
        }
        if (data.recordedSamples != NULL) {
            free(data.recordedSamples);
        }
        Pa_Terminate();
    }

    bool AudioClient::init() {
        // initialize pa
        if (initialized) {
            cerr << "Recorder already initialized" << endl;
            return initialized;
        }
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            paErr(err);
            return initialized;
        }

        printDevices(); // debug

        initialized = true;
        return initialized;
    }

    bool AudioClient::setCaptureDevice(int index) {
        if (!initialized) {
            cerr << "Client not initialized" << endl;
            return false;
        }

        // initialize capture device
        PaStreamParameters params;
        if (index == -1) {
            params.device = Pa_GetDefaultInputDevice();
        } else {
            params.device = index;
        }
        const PaDeviceInfo *info = Pa_GetDeviceInfo(params.device);
        if (params.device == paNoDevice || info == NULL) {
            cerr << "No default capture device" << endl;
            return false;
        }

        params.suggestedLatency = info->defaultLowInputLatency;
        params.hostApiSpecificStreamInfo = NULL;

        captureDevice = AudioDevice();
        captureDevice.params = params;
        // more configuring required by method caller
        return true;
    }

    bool AudioClient::setDefaultCaptureDevice() {
        return setCaptureDevice(-1);
    }

    AudioDevice* AudioClient::getCaptureDevice() {
        return &captureDevice;
    }

    bool AudioClient::isOpened() {
        return initialized && stream != NULL && Pa_IsStreamActive(stream) == 1;
    }

    bool AudioClient::close() {
        // the RPi doesn't seem to like this for some reason
#ifdef __APPLE__
        PaError err = Pa_CloseStream(stream);
        if (err != paNoError) {
            paErr(err);
            return false;
        }
#endif
        return true;
    }

    bool AudioClient::canRecord() {
        if (!canOpen()) {
            cerr << "Cannot open stream" << endl;
            return false;
        }
        if (data.recordedSamples != NULL) {
            cerr << "Sample data already allocated" << endl;
            return false;
        }
        PaError err = Pa_IsFormatSupported(&captureDevice.params, NULL, captureDevice.sampleRate);
        if (err != paNoError) {
            cerr << "Unsupported format" << endl;
            paErr(err);
            return false;
        }
        return true;
    }

    bool AudioClient::record(int seconds, RecordCallback callback) {
        if (!canRecord()) {
            cerr << "Could not start recording" << endl;
            return false;
        }

        PaError     err         = paNoError;
        int         numFrames   = seconds * captureDevice.sampleRate;
        int         numSamples  = numFrames * captureDevice.params.channelCount;
        unsigned    numBytes    = numSamples * sizeof(Sample);

        cout << "Starting new recording" << endl;
        cout << "- Max Seconds: " << seconds << endl;
        cout << "- Channels: " << captureDevice.params.channelCount << endl;
        cout << "- Sample Rate: " << captureDevice.sampleRate << " Hz" << endl;
        cout << "- Sample Format: " << typeid(Sample).name() << sizeof(Sample) * 8 << endl;
        cout << "- Buffer Size: " << captureDevice.bufferSize << endl;
        cout << "- Max Frames: " << numFrames << endl;
        cout << "- Max Samples: " << numSamples << endl;
        cout << "- Max bytes: " << numBytes << endl;

        // initialize data buffer
        data = {};
        data.numFrames = numFrames;
        data.frameIndex = 0;
        data.recordedSamples = (Sample*) malloc(numBytes);
        data.callback = callback;
        if (data.recordedSamples == NULL) {
            cerr << "Could not allocate sample data" << endl;
            return false;
        }
        for (int i = 0; i < numSamples; i++) {
            data.recordedSamples[i] = 0;
        }

        // open input stream
        err = openInputStream(&stream, captureDevice, data);
        if (err != paNoError) {
            paErr(err);
            free(data.recordedSamples);
            return false;
        }

        // set callback for when the stream has been stopped
        err = Pa_SetStreamFinishedCallback(stream, onStreamFinished);
        if (err != paNoError) {
            paErr(err);
            return abortRecord(); // free sample data and abort stream
        }

        // start capturing samples (non-blocking)
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            paErr(err);
            return abortRecord();
        }

        cout << "[recording]" << endl;

        return true;
    }

    bool AudioClient::setPlaybackDevice(int index) {
        // initialize playback device
        if (!initialized) {
            cerr << "Client not initialized" << endl;
            return false;
        }

        PaStreamParameters params;
        if (index == -1) {
            params.device = Pa_GetDefaultOutputDevice();
        } else {
            params.device = index;
        }
        if (params.device == paNoDevice) {
            cerr << "No default playback device" << endl;
            return false;
        }
        const PaDeviceInfo *info = Pa_GetDeviceInfo(params.device);
        if (params.device == paNoDevice || info == NULL) {
            cerr << "No default input device" << endl;
            return false;
        }

        params.suggestedLatency = info->defaultLowOutputLatency;
        params.hostApiSpecificStreamInfo = NULL;

        playbackDevice = AudioDevice();
        playbackDevice.params = params;

        return true;
    }

    bool AudioClient::setDefaultPlaybackDevice() {
        return setPlaybackDevice(-1);
    }

    AudioDevice* AudioClient::getPlaybackDevice() {
        return &playbackDevice;
    }

    bool AudioClient::canPlay() {
        if (!canOpen()) {
            cerr << "Cannot open stream" << endl;
            return false;
        }
        if (data.recordedSamples == NULL) {
            cerr << "Sample data is NULL" << endl;
            return false;
        }
        PaError err = Pa_IsFormatSupported(NULL, &playbackDevice.params, playbackDevice.sampleRate);
        if (err != paNoError) {
            cerr << "Unsupported format" << endl;
            paErr(err);
            return false;
        }
        return true;
    }

    bool AudioClient::play(AudioData &data) {
        if (!canPlay()) {
            cerr << "Could not start playback" << endl;
            return false;
        }

        data.frameIndex = 0;

        cout << "Playing back recording" << endl;
        cout << "- Frames: " << data.numFrames << endl;
        cout << "- Channels: " << playbackDevice.params.channelCount << endl;
        cout << "- Sample Rate: " << playbackDevice.sampleRate << " Hz" << endl;
        cout << "- Sample Format: " << typeid(Sample).name() << sizeof(Sample) * 8 << endl;
        cout << "- Buffer Size: " << playbackDevice.bufferSize << endl;

        PaError err = openOutputStream(&stream, playbackDevice, data);
        if (err != paNoError) {
            paErr(err);
            return false;
        }

        err = Pa_StartStream(stream);
        if (err != paNoError) {
            Pa_AbortStream(stream);
            paErr(err);
            return false;
        }

        cout << "[playing]" << endl;

        // blocking
        while (Pa_IsStreamActive(stream) == 1) {
            Pa_Sleep(100);
        }

        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            Pa_AbortStream(stream);
            paErr(err);
            return false;
        }

        cout << "Stream complete." << endl;
        return true;
    }

    bool AudioClient::canOpen() {
        if (!initialized) {
            cerr << "Client not initialized" << endl;
            return false;
        }
//        if (isOpened()) {
//            cerr << "Stream is active" << endl;
//            return false;
//        }
        if (captureDevice.bufferSize <= 0) {
            cerr << "Invalid buffer size" << endl;
            return false;
        }
        if (captureDevice.sampleRate <= 0) {
            cerr << "Invalid sample rate" << endl;
            return false;
        }
        return true;
    }

    bool AudioClient::abortRecord() {
        Pa_AbortStream(stream);
        free(data.recordedSamples);
        return false;
    }

}
