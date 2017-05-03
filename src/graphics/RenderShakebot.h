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
#ifndef SHAKESPEARE_RENDERSHAKESPEARE_H
#define SHAKESPEARE_RENDERSHAKESPEARE_H

#include "graphics.h"

namespace sb {

    class Shakebot;

    /**
     * Represents the "render" of the main bot.
     */
    class RenderShakebot {

        Shakebot *bot;
        Texture baseTexture;
        Sprite baseSprite;
        Texture mouthTexture;
        IntRect mouthRect;
        Sprite mouthSprite;
        RectangleShape mouthHole;
        Vector2f restingMouthPos;
        float openMouthYOff = 20;

    public:

        RenderShakebot(Shakebot *bot, string textureFile, IntRect mouthRect);

        /**
         * Scales the render by the specified amount.
         *
         * @param scale Scale amount
         */
        void scale(Vector2f scale);

        /**
         * Moves the render by the specified offset.
         *
         * @param v offset amount
         */
        void move(Vector2f v);

        /**
         * Draws this component to the specified window.
         *
         * @param window Window to draw to
         */
        void draw(RenderWindow *window);

    };

}

#endif //SHAKESPEARE_RENDERSHAKESPEARE_H
