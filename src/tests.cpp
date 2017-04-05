#include "tests.h"
#include "Shakebot.h"
#include "audio/record.h"
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

    bool finished = false;

    void onRecordFinish(AudioData* data) {
        cout << "record finished" << endl;
        playAudio(*data);
        finished = true;
    }

    int testPortAudio() {
        cout << "start" << endl;
        sb::startRecording(onRecordFinish);
        //sb::startRecording(onRecordFinish);
        while (!finished);
        //record();
        return 0;
    }

}
