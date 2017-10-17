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
#ifndef SHAKESPEARE_APP_H
#define SHAKESPEARE_APP_H

#include "audio/audio.h"
#include "audio/speech.h"
#include "base64.h"
#include "graphics/graphics.h"
#include "tests.h"
#include "json.hpp"
#include <string>
#include <iostream>

using json = nlohmann::json;
using namespace std;
using namespace sf;

namespace sb {

    class Shakebot;

    /**
     * Returns the bot instance.
     *
     * @return bot instance
     */
    Shakebot* getBot();

    /**
     * Returns the audio client instance.
     *
     * @return Audio client
     */
    AudioClient* getAudioClient();

    /**
     * Returns true if the application is running.
     *
     * @return True if running
     */
    bool isRunning();

    /**
     * Sets whether the application is running.
     *
     * @param running False if should stop
     */
    void setRunning(bool running);

    /**
     * Performs a synchronous request to Google Speech API with the specified
     * AudioData to be translated to text.
     *
     * @param data Data to send to Google
     * @return JSON response from Google
     */
    bool speech2text(const AudioData *data, json &result);

    /**
     * Returns an extract from Wikipedia about the specified subject.
     *
     * @param subject Subject to lookup
     * @param result Wikipedia results
     * @return True if successful
     */
    bool getWikiInfo(const string &subject, json &result);

    /**
     * A utility class for non-assignable types. (i.e. RestClient, AudioClient)
     */
    class NonAssignable {
    public:
        NonAssignable(const NonAssignable&) = delete;
        NonAssignable() = default;
        NonAssignable operator=(const NonAssignable&) = delete;
    };

}

#endif //SHAKESPEARE_APP_H
