#include "RenderShakebot.h"
#include <iostream>

namespace sb {

    RenderShakebot::RenderShakebot(sb::Shakebot *b, string textureFile, sf::IntRect mRect) {
        bot = b;
        mouthRect = mRect;
        restingMouthPos = sf::Vector2f(mRect.left, mRect.top);
        cout << "- Loading texture " << textureFile << endl;
        if (!baseTexture.loadFromFile(textureFile) || !mouthTexture.loadFromFile(textureFile, mouthRect)) {
            cerr << "Error: Failed to load texture from file: " << textureFile << endl;
        }
        baseSprite.setTexture(baseTexture);
        mouthHole.setSize(sf::Vector2f(mRect.width, mRect.height));
        mouthHole.setPosition(sf::Vector2f(mRect.left, mRect.top)); // x
        mouthHole.setFillColor(sf::Color::Black);
        mouthSprite.setTexture(mouthTexture);
        mouthSprite.setPosition(restingMouthPos);
    }

    void RenderShakebot::scale(sf::Vector2f scale) {
        baseSprite.scale(scale);
        mouthSprite.scale(scale);
        mouthHole.scale(scale);
        mouthHole.setPosition(mouthHole.getPosition().x * scale.x, mouthHole.getPosition().y * scale.y);
        restingMouthPos = sf::Vector2f(restingMouthPos.x * scale.x, restingMouthPos.y * scale.y);
    }

    void RenderShakebot::move(sf::Vector2f v) {
        baseSprite.move(v);
        mouthSprite.move(v);
        mouthHole.move(v);
        restingMouthPos = sf::Vector2f(restingMouthPos.x + v.x, restingMouthPos.y + v.y);
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
