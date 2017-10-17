/*
 * RPi-Shakebot
 * ============
 * A voice recognition bot built for Prof. James Eddy's Computer Organization
 * (CS 121) class on the Raspberry Pi 3 Model B.
 *
 * References
 * ~~~~~~~~~~
 * [Festival]           : http://www.cstr.ed.ac.uk/projects/festival/manual/festival_28.html#SEC132
 * [PortAudio]          : http://www.portaudio.com/
 * [SFML]               : https://www.sfml-dev.org/
 * [libcurl]            : https://curl.haxx.se/libcurl/c/
 * [json]               : https://github.com/nlohmann/json
 * [Google Speech API]  : https://cloud.google.com/speech/docs/
 * [MediaWiki API]      : https://www.mediawiki.org/wiki/API:Main_page
 *
 * Copyright (C) Walker Crouse 2017 <wcrouse@uvm.edu>
 */
#include "RenderShakebot.h"
#include "../Shakebot.h"

namespace sb {

    RenderShakebot::RenderShakebot(Shakebot *bot, const string &textureFile, const IntRect &mRect) {
        this->bot = bot;
        mouthRect = mRect;
        restingMouthPos = Vector2f(mRect.left, mRect.top);
        cout << "- Loading texture " << textureFile << endl;
        if (!baseTexture.loadFromFile(textureFile) || !mouthTexture.loadFromFile(textureFile, mouthRect)) {
            cerr << "Error: Failed to load texture from file: " << textureFile << endl;
        }
        baseSprite.setTexture(baseTexture);
        mouthHole.setSize(Vector2f(mRect.width, mRect.height));
        mouthHole.setPosition(Vector2f(mRect.left, mRect.top)); // x
        mouthHole.setFillColor(Color::Black);
        mouthSprite.setTexture(mouthTexture);
        mouthSprite.setPosition(restingMouthPos);
    }

    void RenderShakebot::scale(const Vector2f &scale) {
        baseSprite.scale(scale);
        mouthSprite.scale(scale);
        mouthHole.scale(scale);
        mouthHole.setPosition(mouthHole.getPosition().x * scale.x, mouthHole.getPosition().y * scale.y);
        restingMouthPos = Vector2f(restingMouthPos.x * scale.x, restingMouthPos.y * scale.y);
    }

    void RenderShakebot::move(const Vector2f &v) {
        baseSprite.move(v);
        mouthSprite.move(v);
        mouthHole.move(v);
        restingMouthPos = Vector2f(restingMouthPos.x + v.x, restingMouthPos.y + v.y);
    }

    void RenderShakebot::draw(RenderWindow *window) {
        if (bot->isMouthOpened()) {
            mouthSprite.setPosition(restingMouthPos + Vector2f(0, openMouthYOff));
        } else {
            mouthSprite.setPosition(restingMouthPos);
        }
        window->draw(baseSprite);
        window->draw(mouthHole);
        window->draw(mouthSprite);
    }

}
