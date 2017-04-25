#include "AudioRecorder.h"

#include <iostream>
#include <cassert>

namespace sb {

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData);

    void onStreamFinished(void *audioData);
    void printErr(PaError err);

    AudioRecorder::~AudioRecorder() {
        if (isActive()) {
            Pa_AbortStream(stream);
        }
        if (data.recordedSamples != NULL) {
            free(data.recordedSamples);
        }
        terminate();
    }

    bool AudioRecorder::init() {
        if (this) {
            cerr << "Recorder already initialized" << endl;
            return false;
        }

        PaError err = Pa_Initialize();
        if (err != paNoError) {
            printErr(err);
            return initialized;
        }

        // debug info
        cout << "Available Devices" << endl;
        for (int i = 0; i < Pa_GetDeviceCount(); i++) {
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
            cout << "- " << info->name << endl;
            cout << "    Max Input Channels: " << info->maxInputChannels << endl;
            cout << "    Max Output Channels: " << info->maxOutputChannels << endl;
            cout << "    Default Sample Rate: " << info->defaultSampleRate << endl;
        }

        // initialize input device
        inputParams = {};
        if (deviceIndex == -1) {
            inputParams.device = Pa_GetDefaultInputDevice();
        } else {
            inputParams.device = DEVICE_INDEX;
        }
        if (inputParams.device == paNoDevice) {
            cerr << "No default input device" << endl;
            return terminate();
        }
        const PaDeviceInfo *info = Pa_GetDeviceInfo(inputParams.device);

        cout << "Using Device" << endl;
        cout << "- Name: " << info->name << endl;
        cout << "- Max Input Channels: " << info->maxInputChannels << endl;
        cout << "- Max Output Channels: " << info->maxOutputChannels << endl;
        cout << "- Index: " << inputParams.device << endl;

        inputParams.channelCount = channelCount;
        inputParams.sampleFormat = SAMPLE_FORMAT;
        inputParams.suggestedLatency = info->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = NULL;

        err = Pa_IsFormatSupported(&inputParams, NULL, SAMPLE_RATE);
        if (err != paNoError) {
            cerr << "Unsupported format" << endl;
            printErr(err);
            return terminate();
        }

        initialized = true;
        return initialized;
    }

    bool AudioRecorder::terminate() {
        Pa_Terminate();
        initialized = false;
        return initialized;
    }

    bool AudioRecorder::operator bool() {
        return initialized;
    }

    bool AudioRecorder::isActive() {
        return this && stream != NULL && Pa_IsStreamActive(stream) == 1;
    }

    int AudioRecorder::getSampleRate() const {
        return sampleRate;
    }

    void AudioRecorder::setSampleRate(int sampleRate) {
        assert(sampleRate > 0);
        this->sampleRate = sampleRate;
    }

    int AudioRecorder::getChannelCount() const {
        return channelCount;
    }

    void AudioRecorder::setChannelCount(int channelCount) {
        assert(channelCount > 0);
        this->channelCount = channelCount;
    }

    int AudioRecorder::getDeviceIndex() const {
        return deviceIndex;
    }

    bool AudioRecorder::setDeviceIndex(int deviceIndex) {
        assert(deviceIndex >= 0);
        this->deviceIndex = deviceIndex;
    }

    bool AudioRecorder::record(int seconds) {
        if (!this) {
            cout << "Recorder not initialized" << endl;
            return false;
        }
        if (isActive()) {
            cerr << "Stream is active" << endl;
            return false;
        }

        PaError     err         = paNoError;
        int         numFrames   = seconds * sampleRate;
        int         numSamples  = numFrames * channelCount;
        unsigned    numBytes    = numSamples * sizeof(Sample);

        cout << "Starting new recording" << endl;
        cout << "- Sample Size: " << sizeof(Sample) << endl;
        cout << "- Max Frames: " << numFrames << endl;
        cout << "- Max Samples: " << numSamples << endl;
        cout << "- Max bytes: " << numBytes << endl;

        // initialize data buffer
        if (data.recordedSamples != NULL) {
            cerr << "Sample data already allocated" << endl;
            return false;
        }
        data = {};
        data.numFrames = numFrames;
        data.frameIndex = 0;
        data.recordedSamples = (Sample*) malloc(numBytes);
        if (data.recordedSamples == NULL) {
            cerr << "Could not allocate sample data" << endl;
            return false;
        }
        for (int i = 0; i < numSamples; i++) {
            data.recordedSamples[i] = 0;
        }

        err = Pa_OpenStream(
                &stream,
                &inputParams,
                NULL,
                SAMPLE_RATE,
                FRAMES_PER_BUFFER,
                paClipOff,
                recordCallback,
                &data);
        if (err != paNoError) {
            printErr(err);
            free(data.recordedSamples);
            return false;
        }

        err = Pa_SetStreamFinishedCallback(stream, onStreamFinished);
        if (err != paNoError) {
            printErr(err);
            return abort();
        }

        err = Pa_StartStream(stream);
        if (err != paNoError) {
            printErr(err);
            return abort();
        }

        cout << "[recording]" << endl;

        return true;
    }

    bool AudioRecorder::abort() {
        Pa_AbortStream(stream);
        free(data.recordedSamples);
        return false;
    }

    void printErr(PaError err) {
        cerr << "An error occurred while using the portaudio stream" << endl;
        cerr << "Error number: " << err << endl;
        cerr << "Error message: " << Pa_GetErrorText(err) << endl;
    }

}
