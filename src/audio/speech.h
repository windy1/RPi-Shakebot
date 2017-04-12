#ifndef SHAKESPEARE_VOICE_H
#define SHAKESPEARE_VOICE_H

#include <thread>
#include <string>
#include <future>
#include "record.h"

using namespace std;

namespace sb {

    struct Phrase {
        promise<bool> &pr;
        string str;
    };

    /**
     * Starts the voice thread.
     *
     * @return Voice thread
     */
    void startSpeech();

    /**
     * Stops the voice thread.
     */
    void stopSpeech();

    void joinSpeech();

    /**
     * Pushes a new phrase to be executed on the next iteration of the loop.
     *
     * @param str phrase to execute
     * @param pr  promise to be completed once the phrase has been executed
     */
    void pushSpeech(const string str, promise<bool> &pr);

    string speech2text(const AudioData &data);

}

#endif //SHAKESPEARE_VOICE_H
