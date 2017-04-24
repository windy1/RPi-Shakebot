#ifndef SHAKESPEARE_VOICE_H
#define SHAKESPEARE_VOICE_H

#include <thread>
#include <string>
#include <future>
#include "record.h"

using namespace std;

namespace sb {

    /**
     * A single phrase to perform.
     */
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

    /**
     * Pushes a new phrase to be executed on the next iteration of the loop.
     *
     * @param str phrase to execute
     * @param pr  promise to be completed once the phrase has been executed
     */
    void pushSpeech(const string str, promise<bool> &pr);

}

#endif //SHAKESPEARE_VOICE_H
