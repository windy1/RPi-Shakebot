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
#ifndef SHAKESPEARE_VOICE_H
#define SHAKESPEARE_VOICE_H

#include "audio.h"
#include <thread>
#include <future>

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
    void pushSpeech(string str, promise<bool> &pr);

}

#endif //SHAKESPEARE_VOICE_H
