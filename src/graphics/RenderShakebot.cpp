#include "RenderShakebot.h"
#include <iostream>

namespace sb {

    RenderShakebot::RenderShakebot(sb::Shakebot *b, string textureFile, sf::IntRect mRect) {
        bot = b;
        mouthRect = mRect;
        restingMouthPos = sf::Vector2f(mRect.left * 2, mRect.top * 2);
        if (!baseTexture.loadFromFile(textureFile) || !mouthTexture.loadFromFile(textureFile, mouthRect)) {
            cout << "Error: Failed to load texture from file: " << textureFile << endl;
        }
        baseSprite.setTexture(baseTexture);
        baseSprite.scale(2, 2);
        mouthHole.setSize(sf::Vector2f(mRect.width * 2, mRect.height * 2));
        mouthHole.setPosition(sf::Vector2f(mRect.left * 2, mRect.top * 2));
        mouthHole.setFillColor(sf::Color::Black);
        mouthSprite.setTexture(mouthTexture);
        mouthSprite.scale(2, 2);
        mouthSprite.setPosition(restingMouthPos);
    }

    void RenderShakebot::draw(sf::RenderWindow *window) {
        if (bot->isMouthOpened) {
            mouthSprite.setPosition(restingMouthPos + sf::Vector2f(0, openMouthYOff));
        } else {
            mouthSprite.setPosition(restingMouthPos);
        }
        window->draw(baseSprite);
        window->draw(mouthHole);
        window->draw(mouthSprite);
    }

}
