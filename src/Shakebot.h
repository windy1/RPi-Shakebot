#ifndef SHAKESPEARE_SHAKEBOT_H
#define SHAKESPEARE_SHAKEBOT_H

#include <string>
#include <thread>
#include <SFML/Graphics.hpp>
#include <future>
#include "audio/speech_api.h"
#include "graphics/RenderShakebot.h"

using namespace std;

namespace sb {

    /**
     * Represents the main "bot" or "avatar" the interacts with the end user.
     */
    class Shakebot {

        RenderShakebot render;

        string phraseBuffer;
        string currentWord;
        int wordSyllables = 0;
        int wordUpdates = 0;
        int syllableCount = 0;
        bool firstWord = true;
        bool mouthOpened = false;

        sf::Clock pauseClock;
        sf::Time pauseTime = sf::seconds(0.1);
        sf::Clock delayClock;
        sf::Time delayTime = sf::seconds(0.5);

        bool voxStarted = false;
        bool joinVox = false;
        promise<bool> voxPromise;
        future<bool> voxFuture;

    public:

        Shakebot();

        bool isMouthOpened();

        RenderShakebot* getRender();

        /**
         * Prompts the bot to say the specified phrase.
         *
         * @param phrase Phrase to say
         */
        void say(const string phrase);

        /**
         * Updates the bot. To be called once every application cycle.
         */
        void update();

        void interpret(const AudioData *data);

    };

    /**
     * Recursively estimates the amount of syllables in the specified phrase.
     *
     * @param phrase phrase to count syllables of
     * @return amount of syllables in the phrase
     */
    int countSyllables(const string phrase);

}


#endif //SHAKESPEARE_SHAKEBOT_H
