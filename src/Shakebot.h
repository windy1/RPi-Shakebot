#ifndef SHAKESPEARE_SHAKEBOT_H
#define SHAKESPEARE_SHAKEBOT_H

#include <string>
#include <thread>
#include <SFML/Graphics.hpp>
#include <future>

using namespace std;

namespace sb {

    class Shakebot {

        friend class RenderShakebot;

        string phraseBuffer;
        string currentWord;
        int wordSyllables = 0;
        int wordUpdates = 0;
        int syllableCount = 0;
        bool firstWord = true;
        bool isMouthOpened = false;

        sf::Clock pauseClock;
        sf::Time pauseTime = sf::seconds(0.1);
        sf::Clock delayClock;
        sf::Time delayTime = sf::seconds(0.5);

        bool voxStarted = false;
        bool joinVox = false;
        promise<void> voxPromise;
        future<void> voxFuture;

    public:

        void say(string phrase);

        void update();

    };

    /**
     * Recursively estimates the amount of syllables in the specified phrase.
     *
     * @param phrase phrase to count syllables of
     * @return amount of syllables in the phrase
     */
    int countSyllables(string phrase);

}


#endif //SHAKESPEARE_SHAKEBOT_H
