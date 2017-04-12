#include "tests.h"
#include "Shakebot.h"
#include "audio/record.h"
#include "audio/flac.h"
#include "audio/voice.h"
#include "audio/base64.h"
#include <iostream>
#include <fstream>
#include <festival/festival.h>

using namespace std;

namespace sb {

    int testCountSyllables() {
        int failed = 0;
        vector<double> scores;
        int correct = 0;
        int total = 0;
        for (int syllables = 1; syllables <= 6; syllables++) {
            string fileName = "tests/" + to_string(syllables) + "-syllable-words.txt";
            cout << "**** " << fileName << " ****" << endl;
            ifstream in(fileName);
            if (!in) {
                cout << "Error: Failed to open test file: " << fileName << endl;
                continue;
            }
            string ln;
            int localCorrect = 0;
            int localTotal = 0;
            while (getline(in, ln)) {
                int result = sb::countSyllables(ln);
                if (result == syllables) {
                    // guessed correctly
                    localCorrect++;
                    cout << "[success] ";
                } else {
                    // guessed incorrectly
                    failed++;
                    cout << "[failed] ";
                }
                cout << ln << " : " << result << endl;
                localTotal++;
            }
            scores.push_back(localCorrect / (float) localTotal * 100);
            correct += localCorrect;
            total += localTotal;
            in.close();
        }

        // print stats
        double avg = 0;
        for (int i = 0; i < scores.size(); i++) {
            avg += scores[i];
            cout << i + 1 << " syllables: " << scores[i] << endl;
        }
        cout << "avg = " << avg / scores.size() << endl;
        cout << "total = " << correct / (float) total * 100 << endl;

        // test full phrase
        string phrase = "The quick brown fox jumped over the lazy dog";
        cout << phrase << endl;
        cout << sb::countSyllables(phrase) << " syllables" << endl;

        return failed;
    }

    int testFestival() {
        int failed = 0;
        festival_initialize(true, 210000);
        if (!festival_say_text("Hello world")) {
            failed++;
            cout << "[failed] could not say text" << endl;
        }
        return failed;
    }

    void testEncode8(const AudioData &data) {
        // test 8-bit sample base64 encode and decode
        unsigned int numSamples = (unsigned int) (data.maxFrameIndex * NUM_CHANNELS);
        vector<uint8_t> buffer(numSamples);
        for (int i = 0; i < numSamples; i++) {
            buffer[i] = data.recordedSamples[i];
        }

        string encoded = base64_encode((const unsigned char*) &buffer[0], numSamples);
        ofstream file("tests/testEncode8.txt");
        if (file) {
            file << encoded << endl;
        }
        file.flush();
        file.close();

        // playback decoded data
        const char *decoded = base64_decode(encoded).data();
        AudioData newData;
        newData.frameIndex = 0;
        newData.maxFrameIndex = data.maxFrameIndex;
        newData.recordedSamples = (Sample*) malloc(numSamples);
        for (int i = 0; i < numSamples; i++) {
            newData.recordedSamples[i] = (uint8_t) decoded[i];
        }
        playAudio(newData);
    }

    void testEncode16(const AudioData &data) {
        // FIXME: Google does not seem to like this even though the data can
        // be encoded and decoded properly
        // test 16-bit sample base64 encode and decode
        unsigned int numSamples = (unsigned int) (data.maxFrameIndex * NUM_CHANNELS);
        unsigned int numBytes = numSamples * 2;

        // split each 16-bit int into two 8-bit ints
        vector<uint8_t> buffer(numBytes);
        int sampleIndex = 0;
        for (int i = 0; i < numBytes; i += 2) {
            buffer[i] = (uint8_t) (data.recordedSamples[sampleIndex] >> 0x8);   // high
            buffer[i + 1] = (uint8_t) data.recordedSamples[sampleIndex];        // low
            sampleIndex++;
        }

        // encode bytes to base64
        string encoded = base64_encode((const unsigned char*) &buffer[0], numBytes);
        ofstream file("tests/testEncode16.txt");
        if (file) {
            file << encoded << endl;
        }
        file.flush();
        file.close();

        // decode base64
        string decoded = base64_decode(encoded);
        AudioData newData;
        newData.frameIndex = 0;
        newData.maxFrameIndex = data.maxFrameIndex;
        newData.recordedSamples = (Sample*) malloc(numBytes);
        if (newData.recordedSamples == NULL) {
            cout << "Error: Could not allocate memory for samples." << endl;
            return;
        }

        // reconstruct 16-bit data and playback
        sampleIndex = 0;
        for (int i = 0; i < numBytes; i += 2) {
            newData.recordedSamples[sampleIndex] = ((uint8_t) decoded[i] << 0x8) + (uint8_t) decoded[i + 1];
            sampleIndex++;
        }
        playAudio(newData);
    }

    bool recordFinished = false;

    void onRecordFinish(AudioData* data) {
        cout << "record finished" << endl;
        playAudio(*data);
        encodeFlac(*data, "test/test.flac");
        testEncode8(*data);
        //testEncode16(*data);
        recordFinished = true;
    }

    int testPortAudio() {
        cout << "starting recording" << endl;
        sb::startRecording(onRecordFinish);
        while (!recordFinished);
        return 0;
    }

}
