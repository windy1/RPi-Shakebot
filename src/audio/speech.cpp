#include "speech.h"
#include <queue>

// Reference: http://www.cstr.ed.ac.uk/projects/festival/manual/festival_28.html#SEC132

namespace sb {

    static thread th;
    static bool running = false;
    static queue<Phrase> phraseQueue;

    static void speech() {
        // the main method for the speech thread
        running = true;
        festival_initialize(true, FESTIVAL_HEAP_SIZE);
        festival_say_text("testing 1 2 3");
        cout << "- Speech thread initialized" << endl;
        int i = 0;
        while (running) {
            if (i++ % 1000) {
                cout << "pempty = " << phraseQueue.empty() << endl;
            }
            if (!phraseQueue.empty()) {
                // take next phrase and play it
                Phrase phrase = phraseQueue.front();
                phraseQueue.pop();
                cout << "- Saying: " << phrase.str << endl;
                bool success = true;
                if (!festival_say_text(phrase.str.data())) {
                    cerr << "Error: Could not say text" << endl;
                    success = false;
                }
                phrase.pr.set_value(success);
            }
        }
        cout << "- Shutting down Festival" << endl;
        festival_tidy_up();
    }

    void startSpeech() {
        cout << "- Starting speech thread" << endl;
        th = thread(speech);
    }

    void stopSpeech() {
        cout << "- Stopping speech thread" << endl;
        running = false;
        th.join();
    }

    void pushSpeech(string str, promise<bool> &pr) {
        cout << "- Speech queued: \"" << str << "\"" << endl;
        phraseQueue.push({pr, str});
        cout << "empty = " << phraseQueue.empty() << endl;
    }

}
