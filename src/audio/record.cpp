#include <cstdlib>
#include <iostream>
#include <cassert>
#include "record.h"

using namespace std;

// Reference: http://www.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html

namespace sb {

    AudioData        data;
    PaStream         *stream     = NULL;
    RecordCallback   callback;
    bool             interrupt   = false;

    void printErr(PaError err) {
        cerr << "An error occurred while using the portaudio stream" << endl;
        cerr << "Error number: " << err << endl;
        cerr << "Error message: " << Pa_GetErrorText(err) << endl;
    }

    bool streamAbort(PaError err, AudioData &data) {
        if (err != paNoError) {
            printErr(err);
        }
        if (Pa_IsStreamActive(stream)) {
            Pa_AbortStream(stream);
        }
        if (data.recordedSamples) {
            free(data.recordedSamples);
        }
        Pa_Terminate();
        cerr << "Stream aborted." << endl;
        return false;
    }

    void onStreamFinished(void *audioData) {
        // close stream and pass callback recorded data
        cout << "Stream complete." << endl;
        assert(audioData != NULL);
        // close and terminate pa
        cout << "closing stream" << endl;
        PaError err = /*Pa_CloseStream(stream)*/ paNoError; // FIXME: hangs on RPi
        cout << "stream closed" << endl;
        if (err != paNoError || (err = Pa_Terminate()) != paNoError) {
            cout << "error" << endl;
            printErr(err);
        } else {
            cout << "Executing callback" << endl;
            AudioData data = *(AudioData*) audioData;
            callback(&data);
            // free sample memory
            if (data.recordedSamples) {
                free(data.recordedSamples);
            }
        }
    }

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
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
        Sample *sampleData = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
        long frames;
        int finished;

        if (interrupt) {
            // FIXME
            cout << data->frameIndex << endl;
            cout << "Active recording stream interrupted ";
            cout << "(frame " << to_string(data->frameIndex) << "/";
            cout << to_string(data->numFrames) << ")" << endl;
            interrupt = false;
            return paComplete;
        }

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
                if (NUM_CHANNELS == 2) {
                    *sampleData++ = SAMPLE_SILENCE;
                }
            }
        } else {
            for (long i = 0; i < frames; i++) {
                *sampleData++ = *in++;
                if (NUM_CHANNELS == 2) {
                    *sampleData++ = *in++;
                }
            }
        }

        data->frameIndex += frames;
        return finished;
    }

    bool isActive() {
        return Pa_IsStreamActive(stream) == 1;
    }

    bool startRecording(RecordCallback cback) {
        assert(cback != NULL);
        if (isActive()) {
            cerr << "Stream is active" << endl;
            return false;
        }

        PaError         err         = paNoError;
        int             numFrames   = MAX_SECONDS * SAMPLE_RATE;
        int             numSamples  = numFrames * NUM_CHANNELS;
        unsigned        numBytes    = numSamples * sizeof(Sample);

        cout << "Starting new recording" << endl;
        cout << "- Sample Format: " << typeid(Sample).name() << sizeof(Sample) * 8 << endl;
        cout << "- Channels: " << NUM_CHANNELS << endl;
        cout << "- Sample Rate: " << SAMPLE_RATE << " Hz" << endl;
        cout << "- Buffer Size: " << BUFFER_SIZE << endl;
        cout << "- Max Frames: " << numFrames << endl;
        cout << "- Max Samples: " << numSamples << endl;
        cout << "- Max bytes: " << numBytes << endl;

        // initialize data buffer
        data = {};
        data.numFrames = numFrames;
        data.frameIndex = 0;
        data.recordedSamples = (Sample*) malloc(numBytes);
        cout << &data.recordedSamples << endl;
        if (data.recordedSamples == NULL) {
            cerr << "Could not allocate record array" << endl;
            return streamAbort(err, data);
        }
        for (int i = 0; i < numSamples; i++) {
            data.recordedSamples[i] = 0;
        }

        // initialize portaudio
        err = Pa_Initialize();
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        cout << "Available Devices" << endl;
        for (int i = 0; i < Pa_GetDeviceCount(); i++) {
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
            cout << "- " << info->name << endl;
            cout << "    Max Input Channels: " << info->maxInputChannels << endl;
            cout << "    Max Output Channels: " << info->maxOutputChannels << endl;
            cout << "    Default Sample Rate: " << info->defaultSampleRate << endl;
        }

        // initialize input device
        PaStreamParameters inputParams;
        if (DEVICE_INDEX == -1) {
            inputParams.device = Pa_GetDefaultInputDevice();
        } else {
            inputParams.device = DEVICE_INDEX;
        }
        if (inputParams.device == paNoDevice) {
            cerr << "No default input device" << endl;
            return streamAbort(err, data);
        }
        const PaDeviceInfo *info = Pa_GetDeviceInfo(inputParams.device);

        cout << "Using Device" << endl;
        cout << "- Name: " << info->name << endl;
        cout << "- Max Input Channels: " << info->maxInputChannels << endl;
        cout << "- Max Output Channels: " << info->maxOutputChannels << endl;
        cout << "- Index: " << inputParams.device << endl;

        inputParams.channelCount = NUM_CHANNELS;
        inputParams.sampleFormat = SAMPLE_FORMAT;
        inputParams.suggestedLatency = info->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = NULL;

        err = Pa_IsFormatSupported(&inputParams, NULL, SAMPLE_RATE);
        if (err != paNoError) {
            cerr << "Unsupported format" << endl;
            return streamAbort(err, data);
        }

        // open stream
        err = Pa_OpenStream(
                &stream,
                &inputParams,
                NULL,
                SAMPLE_RATE,
                BUFFER_SIZE,
                paClipOff,
                recordCallback,
                &data);
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        // set callback
        callback = cback;
        err = Pa_SetStreamFinishedCallback(stream, onStreamFinished);
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        // start recording
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        cout << "[recording]" << endl;

        // note: if successful, pa is not terminated so that the stream may
        // continue
        return true;
    }

    bool stopRecording() {
        if (!isActive()) {
            cerr << "No stream active." << endl;
            return false;
        }
        interrupt = true;
        return true;
    }

    int playCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                     const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                     void *userData) {
        // unused
        (void) inputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        // write data to output device
        AudioData *data = (AudioData*) userData;
        Sample *sampleData = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
        Sample *out = (Sample*) outputBuffer;
        int finished;

        unsigned int framesLeft = (unsigned int) (data->numFrames - data->frameIndex);
        if (framesLeft < framesPerBuffer) {
            int i = 0;
            for (i = 0; i < framesLeft; i++) {
                *out++ = *sampleData++;
                if (NUM_CHANNELS == 2) {
                    *out++ = *sampleData++;
                }
            }
            for (; i < framesPerBuffer; i++) {
                *out++ = 0;
                if (NUM_CHANNELS == 2) {
                    *out++ = 0;
                }
            }
            data->frameIndex += framesLeft;
            finished = paComplete;
        } else {
            for (int i = 0; i < framesPerBuffer; i++) {
                *out++ = *sampleData++;
                if (NUM_CHANNELS == 2) {
                    *out++ = *sampleData++;
                }
            }
            data->frameIndex += framesPerBuffer;
            finished = paContinue;
        }

        return finished;
    }

    bool playAudio(const AudioData *audioData) {
        // note: for testing purposes only
        AudioData data = *audioData;
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        data.frameIndex = 0;

        cout << "Playing back recording" << endl;
        cout << "- Frames: " << data.numFrames << endl;

        PaStreamParameters outputParams;
        if (DEVICE_INDEX == -1) {
            outputParams.device = Pa_GetDefaultOutputDevice();
        } else {
            outputParams.device = DEVICE_INDEX;
        }
        if (outputParams.device == paNoDevice) {
            cerr << "No default output device" << endl;
            return streamAbort(err, data);
        }
        outputParams.channelCount = NUM_CHANNELS;
        outputParams.sampleFormat = SAMPLE_FORMAT;
        outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
        outputParams.hostApiSpecificStreamInfo = NULL;

        cout << "Output Device" << endl;
        cout << "- Device Index: " << outputParams.device << endl;
        cout << "- Suggested Latency: " << outputParams.suggestedLatency << endl;

        err = Pa_OpenStream(
                &stream,
                NULL,
                &outputParams,
                SAMPLE_RATE,
                BUFFER_SIZE,
                paClipOff,
                playCallback,
                &data);
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        if (stream) {
            err = Pa_StartStream(stream);
            if (err != paNoError) {
                return streamAbort(err, data);
            }

            cout << "[playing]" << endl;

            // blocking
            while (Pa_IsStreamActive(stream) == 1) {
                Pa_Sleep(100);
            }

            err = Pa_CloseStream(stream);
            if (err != paNoError) {
                return streamAbort(err, data);
            }

            cout << "Stream complete." << endl;
            return true;
        }
        return false;
    }



}
