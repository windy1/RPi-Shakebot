#ifndef SHAKESPEARE_GRAPHICS_H
#define SHAKESPEARE_GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <string>
#include "RenderShakebot.h"

using namespace std;

namespace sb {

    /**
     * Handles graphics within the application.
     */
    class Graphics {

        sf::RenderWindow window;
        sf::Clock clock;
        string nextPhrase;
        sb::RenderShakebot *sbRender;

    public:

        static const string        WINDOW_TITLE;
        static const sf::VideoMode VIDEO_MODE;

        /**
         * Initializes the graphics instance and opens the window.
         */
        void init();

        /**
         * Clears the current window state.
         */
        void clear();

        /**
         * Polls for any new input.
         */
        void pollInput();

        /**
         * Pushes the new state of the window.
         */
        void push();

    };

}

#endif //SHAKESPEARE_GRAPHICS_H
