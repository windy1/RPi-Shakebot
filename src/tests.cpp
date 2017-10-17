/*
 * RPi-Shakebot
 * ============
 * A voice recognition bot built for Prof. James Eddy's Computer Organization
 * (CS 121) class on the Raspberry Pi 3 Model B.
 *
 * References
 * ~~~~~~~~~~
 * [Festival]           : http://www.cstr.ed.ac.uk/projects/festival/manual/festival_28.html#SEC132
 * [PortAudio]          : http://www.portaudio.com/
 * [SFML]               : https://www.sfml-dev.org/
 * [libcurl]            : https://curl.haxx.se/libcurl/c/
 * [json]               : https://github.com/nlohmann/json
 * [Google Speech API]  : https://cloud.google.com/speech/docs/
 * [MediaWiki API]      : https://www.mediawiki.org/wiki/API:Main_page
 *
 * Copyright (C) Walker Crouse 2017 <wcrouse@uvm.edu>
 */
#include "sb.h"
#include "Shakebot.h"
#include "audio/AudioClient.h"
#include "net/RestClient.h"
#include <fstream>
#include <festival/festival.h>

namespace sb {

    static bool             recordFinished  = false;
    static AudioClient      *client         = NULL;
    static const AudioData  *in             = NULL;

    static void onRecordFinish();

    // Shakebot.h
    int testCountSyllables();
    // speech.h
    int testFestival();
    // record.h
    int testPortAudio();

    int runTests() {
        cout << "Running tests..." << endl;
        int failed = 0;
        failed += testCountSyllables();
        failed += testFestival();
        failed += testPortAudio();
        return failed;
    }

    int testCountSyllables() {
        cout << "**** testCountSyllables() ****" << endl;
        int failed = 0;
        vector<double> scores;
        int correct = 0;
        int total = 0;
        for (int syllables = 1; syllables <= 6; syllables++) {
            string fileName = "tests/" + to_string(syllables) + "-syllable-words.txt";
            cout << "- " << fileName << endl;
            ifstream in(fileName);
            if (!in) {
                cerr << "Error: Failed to open test file: " << fileName << endl;
                continue;
            }
            string ln;
            int localCorrect = 0;
            int localTotal = 0;
            while (getline(in, ln)) {
                int result = countSyllables(ln);
                if (result == syllables) {
                    // guessed correctly
                    localCorrect++;
                    cout << "[success] ";
                } else {
                    // guessed incorrectly
                    failed++;
                    cerr << "[failed] ";
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
            cout << "- " << i + 1 << " syllables: " << scores[i] << endl;
        }
        cout << "- avg = " << avg / scores.size() << endl;
        cout << "- total = " << correct / (float) total * 100 << endl;

        // test full phrase
        string phrase = "The quick brown fox jumped over the lazy dog";
        cout << phrase << endl;
        cout << countSyllables(phrase) << " syllables" << endl;
        cout << "Done." << endl;

        return failed;
    }

    int testFestival() {
        cout << "**** testFestival() ****" << endl;
        int failed = 0;
        festival_initialize(true, 840000);
        if (!festival_say_text("supercalifragilisticexpialidocious")) {
            failed++;
            cout << "- [failed] could not say text" << endl;
        }
        festival_tidy_up();
        cout << "Done." << endl;
        return failed;
    }

    int testPortAudio() {
        cout << "**** testPortAudio() ****" << endl;

        cout << "Recording for default time..." << endl;
        int failed = 0;
        client = new AudioClient();
        if (!client->init()) {
            cerr << "Could not initialize client" << endl;
            failed++;
        } else if (!client->setCaptureDevice(DEVICE_INDEX)) {
            cerr << "Could not initialize capture device" << endl;
            failed++;
        } else {
            client->setCaptureCallback(onRecordFinish);
            AudioDevice *device = client->getCaptureDevice();
            device->params.channelCount = CHANNEL_COUNT_CAPTURE;
            device->params.sampleFormat = SAMPLE_FORMAT;
            device->bufferSize = BUFFER_SIZE_CAPTURE;
            cout << *device << endl;
            if (!client->record(MAX_SECONDS)) {
                cerr << "Failed to start recording" << endl;
                failed++;
            }
        }

        while (!recordFinished);

        if (!client->close()) {
            cerr << "Failed to close stream" << endl;
            failed++;
        }

        if (!client->setPlaybackDevice(DEVICE_INDEX)) {
            cerr << "Could not initialize playback device" << endl;
        }

        AudioDevice *device = client->getPlaybackDevice();
        device->params.channelCount = CHANNEL_COUNT_PLAYBACK;
        device->params.sampleFormat = SAMPLE_FORMAT;
        device->bufferSize = BUFFER_SIZE_PLAYBACK;
        cout << *device << endl;
        if (!client->play()) {
            cerr << "Failed to playback" << endl;
        }

        json result;
        if (speech2text(in, result)) {
            cout << result.dump(4) << endl;
        }

        cout << "Done." << endl;
        return failed;
    }

    void onRecordFinish() {
        cout << "- Finished recording" << endl;
        in = client->data();
        recordFinished = true;
    }

    int testWikiApi() {
        cout << "**** testWikiApi ****" << endl;
        json result;
        if (!getWikiInfo("Phish", result)) {
            cerr << "Could not make wiki request" << endl;
            return 1;
        }

        festival_initialize(true, 840000);
        for (auto &page : result["query"]["pages"]) {
            cout << page.dump(4) << endl;
            EST_String str = page["extract"].get<string>().c_str();
            festival_say_text(str);
        }
        festival_tidy_up();

        return 0;
    }

}
