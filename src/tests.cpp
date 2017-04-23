#include "tests.h"
#include "Shakebot.h"
#include "audio/record.h"
#include "audio/flac.h"
#include "audio/speech.h"
#include <fstream>
#include <festival/festival.h>

using namespace std;

namespace sb {

    static bool recordFinished = false;
    static void onRecordFinish(const AudioData* data);

    // Shakebot.h
    int testCountSyllables();
    // speech.h
    int testFestival();
    // record.h
    int testPortAudio();

    int runTests() {
        cout << "Running tests..." << endl;
        int failed = 0;
        failed += sb::testCountSyllables();
        failed += sb::testFestival();
        failed += sb::testPortAudio();
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
                int result = sb::countSyllables(ln);
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
        cout << sb::countSyllables(phrase) << " syllables" << endl;
        cout << "Done." << endl;

        return failed;
    }

    int testFestival() {
        cout << "**** testFestival() ****" << endl;
        int failed = 0;
        festival_initialize(true, 210000);
        if (!festival_say_text("Hello world")) {
            failed++;
            cout << "- [failed] could not say text" << endl;
        }
        cout << "Done." << endl;
        return failed;
    }

    int testPortAudio() {
        cout << "**** testPortAudio() ****" << endl;

        cout << "Recording for default time..." << endl;
        int failed = 0;
        if (!sb::startRecording(onRecordFinish)) {
            cout << "- [failed] could not start recording" << endl;
            failed++;
            recordFinished = true;
        }
        while (!recordFinished);

        cout << "Done." << endl;
        return failed;
    }

    void onRecordFinish(const AudioData *data) {
        cout << "- Finished recording" << endl;
        playAudio(data);
        //encodeFlac(*data, "test/test.flac");
        json result;
        if (speech2text(data, result)) {
            cout << result.dump(4) << endl;
        }
        recordFinished = true;
    }

}
