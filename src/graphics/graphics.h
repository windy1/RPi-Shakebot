#ifndef SHAKESPEARE_GRAPHICSX_H
#define SHAKESPEARE_GRAPHICSX_H

#include <SFML/Graphics/RenderWindow.hpp>

namespace sb {

    void initWindow();

    void pollInput();

    void display();

    sf::RenderWindow* getWindow();

    void setFullScreen(bool fullScreen);

}

#endif //SHAKESPEARE_GRAPHICSX_H
