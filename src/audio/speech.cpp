#include "speech.h"
#include <queue>
#include <festival/festival.h>

using namespace std;

namespace sb {

    static thread th;
    static bool running = false;
    static queue<Phrase> phraseQueue;
    static string *responseBody;

    static void speech() {
        // the main method for the speech thread
        running = true;
        festival_initialize(true, 210000);
        while (running) {
            if (!phraseQueue.empty()) {
                // take next phrase and play it
                Phrase phrase = phraseQueue.front();
                phraseQueue.pop();
                bool success = true;
                if (!festival_say_text(phrase.str.data())) {
                    cout << "Error: Could not say text" << endl;
                    success = false;
                }
                phrase.pr.set_value(success);
            }
        }
        festival_tidy_up();
    }

    void startSpeech() {
        th = thread(speech);
    }

    void stopSpeech() {
        running = false;
        th.join();
    }

    void pushSpeech(const string str, promise<bool> &pr) {
        phraseQueue.push({pr, str});
    }

}
