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
#ifndef SHAKESPEARE_GRAPHICSX_H
#define SHAKESPEARE_GRAPHICSX_H

#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

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
    RenderWindow* getWindow();

    /**
     * Sets if the window should be opened in full screen.
     *
     * @param fullScreen True if full screen
     */
    void setFullScreen(bool fullScreen);

}

#endif //SHAKESPEARE_GRAPHICSX_H
