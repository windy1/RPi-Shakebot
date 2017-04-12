#include "voice.h"
#include <queue>
#include <festival/festival.h>
#include <cassert>
#include "base64.h"

using namespace std;

namespace sb {

    bool running = false;
    queue<Phrase> toSay;

    void run() {
        running = true;
        festival_initialize(true, 210000);
        while (running) {
            if (!toSay.empty()) {
                Phrase phrase = toSay.front();
                toSay.pop();
                if (!festival_say_text(phrase.str.data())) {
                    cout << "Error: Could not say text" << endl;
                }
                phrase.pr.set_value();
            }
        }
    }

    thread startVoiceThread() {
        return thread(run);
    }

    void stopVoiceThread() {
        running = false;
    }

    void toVoice(const string str, promise<void> &pr) {
        toSay.push({pr, str});
    }

}
