#ifndef SHAKESPEARE_RENDERSHAKESPEARE_H
#define SHAKESPEARE_RENDERSHAKESPEARE_H

#include <SFML/Graphics.hpp>
#include <string>
#include "../Shakebot.h"

using namespace std;

namespace sb {

    /**
     * Represents the "render" of the main bot.
     */
    class RenderShakebot {

        sb::Shakebot *bot;
        sf::Texture baseTexture;
        sf::Sprite baseSprite;
        sf::Texture mouthTexture;
        sf::IntRect mouthRect;
        sf::Sprite mouthSprite;
        sf::RectangleShape mouthHole;
        sf::Vector2f restingMouthPos;
        float openMouthYOff = 35;

    public:

        RenderShakebot(sb::Shakebot *bot, string textureFile, sf::IntRect mouthRect);

        /**
         * Draws this component to the specified window.
         *
         * @param window Window to draw to
         */
        void draw(sf::RenderWindow *window);

    };

}

#endif //SHAKESPEARE_RENDERSHAKESPEARE_H
