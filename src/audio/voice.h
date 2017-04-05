#ifndef SHAKESPEARE_VOICE_H
#define SHAKESPEARE_VOICE_H

#include <thread>
#include <string>
#include <future>

using namespace std;

namespace sb { namespace vox {

    struct Phrase {
        promise<void> &pr;
        string str;
    };

    /**
     * Starts the voice thread.
     *
     * @return Voice thread
     */
    thread start();

    /**
     * Stops the voice thread.
     */
    void stop();

    /**
     * Pushes a new phrase to be executed on the next iteration of the loop.
     *
     * @param str phrase to execute
     * @param pr  promise to be completed once the phrase has been executed
     */
    void push(string str, promise<void> &pr);

}}

#endif //SHAKESPEARE_VOICE_H
