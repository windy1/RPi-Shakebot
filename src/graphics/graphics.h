#ifndef SHAKESPEARE_GRAPHICSX_H
#define SHAKESPEARE_GRAPHICSX_H

#include <SFML/Graphics/RenderWindow.hpp>

namespace sb {

    /**
     * Initializes and creates the window.
     */
    void initWindow();

    /**
     * Handles all new input events.
     */
    void pollInput();

    /**
     * Pushes the next display cycle to the window.
     */
    void display();

    /**
     * Returns the opened window.
     *
     * @return Opened window
     */
    sf::RenderWindow* getWindow();

    /**
     * Sets if the window should be opened in full screen.
     *
     * @param fullScreen True if full screen
     */
    void setFullScreen(bool fullScreen);

}

#endif //SHAKESPEARE_GRAPHICSX_H
