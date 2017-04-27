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

namespace sb {

    class Shakebot;

    /**
     * Returns the bot instance.
     *
     * @return bot instance
     */
    sb::Shakebot* getBot();

    /**
     * Returns the audio client instance.
     *
     * @return Audio client
     */
    sb::AudioClient* getAudioClient();

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

    bool getWikiInfo(string subject, json &result);

    class NonAssignable {
        NonAssignable(const NonAssignable&) = delete;
        NonAssignable operator=(const NonAssignable&) = delete;
    public:
        NonAssignable() {}
    };

}

#endif //SHAKESPEARE_APP_H
