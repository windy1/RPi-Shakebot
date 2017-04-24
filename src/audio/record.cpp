#include <cstdlib>
#include <iostream>
#include <cassert>
#include "record.h"

using namespace std;

// Reference: http://www.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html

namespace sb {

    AudioData        data;
    PaStream         *stream     = NULL;
    RecordCallback   *callback   = NULL;
    bool             interrupt   = false;

    void printErr(PaError err);
    bool streamAbort(PaError err, AudioData &data);
    void onStreamFinished(void *audioData);

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData);

    bool isActive() {
        return Pa_IsStreamActive(stream) == 1;
    }

    bool startRecording(RecordCallback *cback) {
        assert(cback != NULL);
        if (isActive()) {
            cerr << "Stream is active" << endl;
            return false;
        }

        PaError         err         = paNoError;
        int             numFrames   = MAX_SECONDS * SAMPLE_RATE;
        int             numSamples  = numFrames * NUM_CHANNELS;
        unsigned int    numBytes    = numSamples * sizeof(Sample);

        cout << "Starting new recording" << endl;
        cout << "- Sample Size: " << sizeof(Sample) << endl;
        cout << "- Max Frames: " << numFrames << endl;
        cout << "- Max Samples: " << numSamples << endl;
        cout << "- Max bytes: " << numBytes << endl;

        // initialize data buffer
        data = {};
        data.maxFrameIndex = numFrames;
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

        // initialize input device
        PaStreamParameters inputParams;
        inputParams.device = Pa_GetDefaultInputDevice();
        if (inputParams.device == paNoDevice) {
            cerr << "No default input device" << endl;
            return streamAbort(err, data);
        }
        inputParams.channelCount = NUM_CHANNELS;
        inputParams.sampleFormat = SAMPLE_FORMAT;
        inputParams.suggestedLatency = Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = NULL;

        cout << "Input Device" << endl;
        cout << "- Device Index: " << inputParams.device << endl;
        cout << "- Suggested Latency: " << inputParams.suggestedLatency << endl;

        // open stream
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

    void onStreamFinished(void *audioData) {
        // close stream and pass callback recorded data
        assert(callback != NULL);
        assert(audioData != NULL);
        // close and terminate pa
        PaError err = Pa_CloseStream(stream);
        if (err != paNoError || (err = Pa_Terminate()) != paNoError) {
            printErr(err);
        } else {
            AudioData data = *(AudioData*) audioData;
            cout << "Stream complete." << endl;
            callback(&data);
            // free sample memory
            if (data.recordedSamples) {
                free(data.recordedSamples);
            }
        }
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

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData) {
        // unused
        (void) outputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        AudioData *data = (AudioData*) userData;
        const Sample *in = (const Sample*) inputBuffer;
        // sampleData starts at current frame
        Sample *sampleData = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
        long frames;
        int finished;

        cout << ".";
        if (interrupt) {
            // FIXME
            cout << data->frameIndex << endl;
            cout << "Active recording stream interrupted ";
            cout << "(frame " << to_string(data->frameIndex) << "/";
            cout << to_string(data->maxFrameIndex) << ")" << endl;
            interrupt = false;
            return paComplete;
        }

        unsigned long framesLeft = (unsigned long) (data->maxFrameIndex - data->frameIndex);
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

    bool stopRecording() {
        if (!isActive()) {
            cerr << "No stream active." << endl;
            return false;
        }
        interrupt = true;
        return true;
    }

    void printErr(PaError err) {
        cerr << "An error occurred while using the portaudio stream" << endl;
        cerr << "Error number: " << err << endl;
        cerr << "Error message: " << Pa_GetErrorText(err) << endl;
    }

    int playCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                     const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                     void *userData);

    bool playAudio(const AudioData *audioData) {
        AudioData data = *audioData;
        // note: for testing purposes only
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            return streamAbort(err, data);
        }

        data.frameIndex = 0;

        cout << "Playing back recording" << endl;
        cout << "- Frames: " << data.maxFrameIndex << endl;

        PaStreamParameters outputParams;
        outputParams.device = Pa_GetDefaultOutputDevice();
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
                FRAMES_PER_BUFFER,
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

        unsigned int framesLeft = (unsigned int) (data->maxFrameIndex - data->frameIndex);
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

}
