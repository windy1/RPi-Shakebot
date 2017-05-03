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
#include "graphics.h"
#include "../sb.h"
#include "../Shakebot.h"
#include "../audio/AudioClient.h"

namespace sb {

    static const VideoMode  DEFAULT_VIDEO_MODE      (440 * 2, 564 * 2);
    static const string     WINDOW_TITLE        =   "Shakebot 1.0";
    static const bool       MOUSE_VISIBLE       =   false;
    static const bool       KEY_REPEAT          =   false;

    static VideoMode    videoMode  = DEFAULT_VIDEO_MODE;
    static bool         fullScreen = false;
    static RenderWindow window;

    void initWindow() {
        Uint32 style = fullScreen ? Style::Fullscreen : Style::Default;
        cout << "- Creating window" << endl;
        window.create(videoMode, WINDOW_TITLE, style);
        window.setMouseCursorVisible(MOUSE_VISIBLE);
        window.setKeyRepeatEnabled(KEY_REPEAT);
    }

    void onRecordFinished() {
        // Note: Called from PA thread
        cout << "Recording Complete." << endl;
        getBot()->interpret(getAudioClient()->data());
    }

    void pollInput() {
        Event event;
        AudioClient *audio = getAudioClient();
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed:
                    window.close();
                    break;
                case Event::MouseButtonPressed:
                    audio->setCaptureCallback(onRecordFinished);
                    audio->record(MAX_SECONDS);
                    break;
                case Event::MouseButtonReleased:
                    audio->close();
                    break;
                default:
                    break;
            }
        }
    }

    void display() {
        window.display();
        setRunning(window.isOpen());
    }

    RenderWindow* getWindow() {
        return &window;
    }

    void setFullScreen(bool fs) {
        fullScreen = fs;
        if (fullScreen) {
            videoMode = VideoMode::getFullscreenModes()[0];
        } else {
            videoMode = DEFAULT_VIDEO_MODE;
        }
    }

}
