#include "AudioClient.h"

#include <iostream>
#include <cassert>

namespace sb {

    int rc(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData);

    void onStreamFinished(void *audioData);
    void perr(PaError err);
    void printDevices();

    AudioClient::~AudioClient() {
        // cleanup
        if (isActive()) {
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
            return false;
        }
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            perr(err);
            return initialized;
        }

        printDevices(); // debug

        initialized = true;
        return initialized;
    }

    bool AudioClient::setCaptureDevice(int index) {
        // initialize capture device
        PaStreamParameters params;
        params = {};
        if (index == -1) {
            params.device = Pa_GetDefaultInputDevice();
        } else {
            params.device = index;
        }
        if (params.device == paNoDevice) {
            cerr << "No default capture device" << endl;
            return false;
        }
        index = params.device;

        params.suggestedLatency = Pa_GetDeviceInfo(index)->defaultLowInputLatency;
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

    bool AudioClient::isActive() {
        return initialized && stream != NULL && Pa_IsStreamActive(stream) == 1;
    }

    bool AudioClient::canRecord() {
        if (!initialized) {
            cerr << "Client not initialized" << endl;
            return false;
        }
        if (isActive()) {
            cerr << "Stream is active" << endl;
            return false;
        }
        if (data.recordedSamples != NULL) {
            cerr << "Sample data already allocated" << endl;
            return false;
        }
        if (captureDevice.bufferSize <= 0) {
            cerr << "Invalid buffer size" << endl;
            return false;
        }
        if (captureDevice.sampleRate <= 0) {
            cerr << "Invalid sample rate" << endl;
            return false;
        }
        PaError err = Pa_IsFormatSupported(&captureDevice.params, NULL, captureDevice.sampleRate);
        if (err != paNoError) {
            cerr << "Unsupported format" << endl;
            perr(err);
            return false;
        }
        return true;
    }

    bool AudioClient::record(int seconds) {
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
        if (data.recordedSamples == NULL) {
            cerr << "Could not allocate sample data" << endl;
            return false;
        }
        for (int i = 0; i < numSamples; i++) {
            data.recordedSamples[i] = 0;
        }

        // open input stream
        err = Pa_OpenStream(
                &stream,                    //
                &captureDevice.params,      //
                NULL,                       // output stream
                captureDevice.sampleRate,   //
                captureDevice.bufferSize,   //
                paClipOff,                  // disable clipping
                rc,                         // callback to read samples
                &data);                     // the data object to read into
        if (err != paNoError) {
            perr(err);
            free(data.recordedSamples);
            return false;
        }

        // set callback for when the stream has been stopped
        err = Pa_SetStreamFinishedCallback(stream, onStreamFinished);
        if (err != paNoError) {
            perr(err);
            return abort(); // free sample data and abort stream
        }

        // start capturing samples (non-blocking)
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            perr(err);
            return abort();
        }

        cout << "[recording]" << endl;

        return true;
    }

    bool AudioClient::abort() {
        Pa_AbortStream(stream);
        free(data.recordedSamples);
        return false;
    }

    void perr(PaError err) {
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

    int rc(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData) {
        // unused
        (void) outputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        //cout << ".";

        AudioData *data = (AudioData*) userData;
        const Sample *in = (const Sample*) inputBuffer;
        // sampleData starts at current frame
        Sample *sampleData = &data->recordedSamples[data->frameIndex * CHANNEL_COUNT_CAPTURE];
        long frames;
        int finished;

//        if (interrupt) {
//            // FIXME
//            cout << data->frameIndex << endl;
//            cout << "Active recording stream interrupted ";
//            cout << "(frame " << to_string(data->frameIndex) << "/";
//            cout << to_string(data->numFrames) << ")" << endl;
//            interrupt = false;
//            return paComplete;
//        }

        unsigned long framesLeft = (unsigned long) (data->numFrames - data->frameIndex);
        if (framesLeft < framesPerBuffer) {
            // write the rest of the frames
            frames = framesLeft;
            finished = paComplete;
        } else {
            // write the standard amount of frames
            frames = framesPerBuffer;
            finished = paContinue;
        }

        // take samples
        if (inputBuffer == NULL) {
            // no new data
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
        return finished;
    }

}
