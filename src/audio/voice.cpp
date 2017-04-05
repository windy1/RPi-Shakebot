#include "voice.h"
#include <queue>
#include <festival/festival.h>

namespace sb { namespace vox {

    bool running = false;
    queue<sb::vox::Phrase> toSay;

    void run() {
        running = true;
        festival_initialize(true, 210000);
        while (running) {
            if (!toSay.empty()) {
                sb::vox::Phrase phrase = toSay.front();
                toSay.pop();
                if (!festival_say_text(phrase.str.data())) {
                    cout << "Error: Could not say text" << endl;
                }
                phrase.pr.set_value();
            }
        }
    }

    thread start() {
        return thread(run);
    }

    void stop() {
        running = false;
    }

    void push(string str, promise<void> &pr) {
        toSay.push({pr, str});
    }

}}
