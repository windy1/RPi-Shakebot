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
#include "speech.h"
#include <queue>

namespace sb {

    static thread th;
    static bool running = false;
    static queue<Phrase> phraseQueue;

    bool lock = true;
    bool debug = false;

    static void speech() {
        // the main method for the speech thread
        running = true;
        festival_initialize(true, FESTIVAL_HEAP_SIZE);
        cout << "- Speech thread initialized" << endl;
        while (running) {
            if (debug) {
                cout << "lock " << lock << endl;
                cout << "empty " << phraseQueue.empty() << endl;
            }
            if (lock) {
                continue;
            }
            debug = false;
            bool empty = phraseQueue.empty();
            if (!empty) {
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
        debug = true;
        lock = true;
        phraseQueue.push({pr, str});
        lock = false;
    }

}
