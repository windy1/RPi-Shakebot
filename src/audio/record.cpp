#include <cstdlib>
#include <iostream>
#include <cassert>
#include "record.h"

using namespace std;

// Reference: http://www.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html

namespace sb {

    static RecordCallback *callback = nullptr;
    static PaStream *stream;
    static bool interrupt = false;

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData);

    int playCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                     const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                     void *userData);

    void onStreamFinished(void *audioData);

    void printErr(PaError err);

    bool startRecording(RecordCallback *cback) {
        // initialize buffer
        PaError err = paNoError;
        bool success = true;
        AudioData data;
        data.frameIndex = 0;
        int numFrames = data.maxFrameIndex = MAX_SECONDS * SAMPLE_RATE;
        int numSamples = numFrames * NUM_CHANNELS;
        unsigned int numBytes = numSamples * sizeof(Sample);

        cout << "sample size = " << sizeof(Sample) << endl;
        cout << "numFrames = " << numFrames << endl;
        cout << "numSamples = " << numSamples << endl;
        cout << "numBytes = " << numBytes << endl;

        data.recordedSamples = (Sample*) malloc(numBytes);
        if (data.recordedSamples == NULL) {
            cout << "Error: Could not allocate record array." << endl;
            success = false;
            goto done;
        }
        for (int i = 0; i < numSamples; i++) {
            data.recordedSamples[i] = 0;
        }

        // initialize portaudio
        err = Pa_Initialize();
        if (err != paNoError) {
            goto done;
        }

        // make sure stream is not active
        if (Pa_IsStreamActive(stream) > 0) {
            cout << "Error: Stream is active" << endl;
            success = false;
            goto done;
        }

        // initialize input device
        PaStreamParameters inputParams;
        inputParams.device = Pa_GetDefaultInputDevice();
        if (inputParams.device == paNoDevice) {
            cout << "Error: No default input device." << endl;
            success = false;
            goto done;
        }
        inputParams.channelCount = NUM_CHANNELS;
        inputParams.sampleFormat = SAMPLE_FORMAT;
        inputParams.suggestedLatency = Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = NULL;

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
            goto done;
        }

        // set callback
        callback = cback;
        err = Pa_SetStreamFinishedCallback(stream, onStreamFinished);
        if (err != paNoError) {
            goto done;
        }

        // start recording
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            goto done;
        }

        done:
        // cleanup
        if(err != paNoError) {
            printErr(err);
            success = false;
        }
        if (!success) {
            // terminate stream if active
            if (Pa_IsStreamActive(stream)) {
                Pa_CloseStream(stream);
            }
            // free sample memory
            if(data.recordedSamples) {
                free(data.recordedSamples);
            }
            // terminate port audio
            Pa_Terminate();
        }
        return success;
    }

    void onStreamFinished(void *audioData) {
        // close stream and pass callback recorded data
        assert(callback != NULL);
        PaError err = Pa_CloseStream(stream);
        if (err != paNoError || (err = Pa_Terminate()) != paNoError) {
            printErr(err);
        } else {
            AudioData *data = (AudioData*) audioData;
            callback(data);
            // free sample memory
            if (data->recordedSamples) {
                free(data->recordedSamples);
            }
        }
    }

    bool stopRecording() {
        // TODO
        return false;
    }

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData) {
        // unused
        (void) outputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        // write new frames to data
        AudioData *data = (AudioData*) userData;
        const Sample *in = (const Sample*) inputBuffer;
        // sampleData starting at current frame
        Sample *sampleData = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
        long frames;
        int finished;
        unsigned long framesLeft = (unsigned long) (data->maxFrameIndex - data->frameIndex);
        if (framesLeft < framesPerBuffer) {
            frames = framesLeft;
            finished = paComplete;
        } else {
            frames = framesPerBuffer;
            finished = paContinue;
        }
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

    void printErr(PaError err) {
        cout << "An error occurred while using the portaudio stream" << endl;
        cout << "Error number: " << err << endl;
        cout << "Error message: " << Pa_GetErrorText(err) << endl;
    }

    bool playAudio(AudioData &data) {
        bool success = true;
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            goto done;
        }

        data.frameIndex= 0;

        PaStreamParameters outputParams;
        outputParams.device = Pa_GetDefaultOutputDevice();
        if (outputParams.device == paNoDevice) {
            cout << "Error: No default output device." << endl;
            success = false;
            goto done;
        }
        outputParams.channelCount = NUM_CHANNELS;
        outputParams.sampleFormat = SAMPLE_FORMAT;
        outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
        outputParams.hostApiSpecificStreamInfo = NULL;

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
            goto done;
        }

        if (stream) {
            err = Pa_StartStream(stream);
            if (err != paNoError) {
                goto done;
            }
            cout << "Playing back..." << endl;

            // blocking
            while (Pa_IsStreamActive(stream) == 1) {
                Pa_Sleep(100);
            }

            err = Pa_CloseStream(stream);
            if (err != paNoError) {
                goto done;
            }
            cout << "Done." << endl;
        }

        done:
        if (err != paNoError) {
            printErr(err);
            success = false;
        }
        Pa_Terminate();
        return success;
    }

}
