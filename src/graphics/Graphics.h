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

        static const sf::VideoMode DEFAULT_VIDEO_MODE;

        sf::RenderWindow window;
        sf::VideoMode videoMode = DEFAULT_VIDEO_MODE;
        sf::Clock clock;
        string nextPhrase;
        sb::RenderShakebot *sbRender;
        bool fullScreen = false;

    public:

        static const string        WINDOW_TITLE;

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

        sb::RenderShakebot* getBotRender() const;

        void setFullScreen(bool fullScreen);

    };

}

#endif //SHAKESPEARE_GRAPHICS_H
