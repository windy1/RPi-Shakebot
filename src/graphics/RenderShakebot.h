#ifndef SHAKESPEARE_RENDERSHAKESPEARE_H
#define SHAKESPEARE_RENDERSHAKESPEARE_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

namespace sb {

    class Shakebot;

    /**
     * Represents the "render" of the main bot.
     */
    class RenderShakebot {

        Shakebot *bot;
        sf::Texture baseTexture;
        sf::Sprite baseSprite;
        sf::Texture mouthTexture;
        sf::IntRect mouthRect;
        sf::Sprite mouthSprite;
        sf::RectangleShape mouthHole;
        sf::Vector2f restingMouthPos;
        float openMouthYOff = 20;

    public:

        RenderShakebot(Shakebot *bot, string textureFile, sf::IntRect mouthRect);

        /**
         * Scales the render by the specified amount.
         *
         * @param scale Scale amount
         */
        void scale(sf::Vector2f scale);

        /**
         * Moves the render by the specified offset.
         *
         * @param v offset amount
         */
        void move(sf::Vector2f v);

        /**
         * Draws this component to the specified window.
         *
         * @param window Window to draw to
         */
        void draw(sf::RenderWindow *window);

    };

}

#endif //SHAKESPEARE_RENDERSHAKESPEARE_H
