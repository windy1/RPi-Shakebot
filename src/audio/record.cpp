#include <cstdlib>
#include <iostream>
#include <cassert>
#include "record.h"

using namespace std;

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

    /*
     * Credit due to the example files included with the PortAudio library (paex_record.c).
     * http://www.portaudio.com/docs/v19-doxydocs/paex__record_8c.html
     */

    bool startRecording(RecordCallback cback) {
        // initialize buffer
        cout << "startRecording()" << endl;
        PaError err = paNoError;
        bool success = true;
        AudioData data;
        data.frameIndex = 0;
        int numFrames = data.maxFrameIndex = MAX_SECONDS * SAMPLE_RATE;
        int numSamples = numFrames * NUM_CHANNELS;
        unsigned int numBytes = numSamples * sizeof(Sample);

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
        inputParams.sampleFormat = paFloat32;
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
            cout << "An error occured while using the portaudio stream" << endl;
            cout << "Error number: " << err << endl;
            cout << "Error message: " << Pa_GetErrorText(err) << endl;
            success = false;
        }
        if (!success) {
            if (Pa_IsStreamActive(stream)) {
                Pa_CloseStream(stream);
            }
            if(data.recordedSamples) {
                free(data.recordedSamples);
            }
            Pa_Terminate();
        }
        return success;
    }

    void onStreamFinished(void *audioData) {
        cout << "stream finished, closing stream" << endl;
        assert(callback != nullptr);
        PaError err = Pa_CloseStream(stream);
        if (err != paNoError || (err = Pa_Terminate()) != paNoError) {
            printErr(err);
        } else {
            AudioData *data = (AudioData*) audioData;
            callback(data);
            if (data->recordedSamples) {
                free(data->recordedSamples);
            }
        }
    }

    bool stopRecording() {
        return false;
    }

    int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                       void *userData) {
        cout << "recordCallback()" << endl;
        AudioData *data = (AudioData*) userData;
        const Sample *in = (const Sample*) inputBuffer;
        Sample *buffer = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
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
            for (long i = 0; i < frames; i++) {
                *buffer++ = SAMPLE_SILENCE;
                if (NUM_CHANNELS == 2) {
                    *buffer++ = SAMPLE_SILENCE;
                }
            }
        } else {
            for (long i = 0; i < frames; i++) {
                *buffer++ = *in++;
                if (NUM_CHANNELS == 2) {
                    *buffer++ = *in++;
                }
            }
        }
        data->frameIndex += frames;
        return finished;
    }

    int playCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                     const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                     void *userData) {
        AudioData *data = (AudioData*) userData;
        Sample *out = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
        Sample *buffer = (Sample*) outputBuffer;
        int finished;
        unsigned int framesLeft = (unsigned int) (data->maxFrameIndex - data->frameIndex);
        if (framesLeft < framesPerBuffer) {
            int i = 0;
            for (i = 0; i < framesLeft; i++) {
                *buffer++ = *out++;
                if (NUM_CHANNELS == 2) {
                    *buffer++ = *out++;
                }
            }
            for (; i < framesPerBuffer; i++) {
                *buffer++ = 0;
                if (NUM_CHANNELS == 2) {
                    *buffer++ = 0;
                }
            }
            data->frameIndex += framesLeft;
            finished = paComplete;
        } else {
            for (int i = 0; i < framesPerBuffer; i++) {
                *buffer++ = *out++;
                if (NUM_CHANNELS == 2) {
                    *buffer++ = *out++;
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

    void playAudio(AudioData &data) {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            goto done;
        }

        data.frameIndex= 0;

        PaStreamParameters outputParams;
        outputParams.device = Pa_GetDefaultOutputDevice();
        if (outputParams.device == paNoDevice) {
            cout << "Error: No default output device." << endl;
            goto done;
        }
        outputParams.channelCount = NUM_CHANNELS;
        outputParams.sampleFormat = paFloat32;
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
            cout << "Playing back" << endl;

            while (Pa_IsStreamActive(stream) == 1) {
                Pa_Sleep(100);
            }

            err = Pa_CloseStream(stream);
            if (err != paNoError) {
                goto done;
            }
            cout << "Done" << endl;
        }

        done:
        Pa_Terminate();
        if (err != paNoError) {
            printErr(err);
        }
    }

}
