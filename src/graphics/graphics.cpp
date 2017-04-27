#include "graphics.h"
#include "../sb.h"
#include "../Shakebot.h"
#include "../audio/AudioClient.h"

namespace sb {

    static const sf::VideoMode  DEFAULT_VIDEO_MODE  (440 * 2, 564 * 2);
    static const string         WINDOW_TITLE        = "Shakebot 1.0";
    static const bool           MOUSE_VISIBLE       = false;
    static const bool           KEY_REPEAT          = false;

    static sf::VideoMode    videoMode   = DEFAULT_VIDEO_MODE;
    static bool             fullScreen  = false;
    static sf::RenderWindow window;

    void initWindow() {
        sf::Uint32 style = fullScreen ? sf::Style::Fullscreen : sf::Style::Default;
        cout << "- Creating window" << endl;
        window.create(videoMode, WINDOW_TITLE, style);
        window.setMouseCursorVisible(MOUSE_VISIBLE);
        window.setKeyRepeatEnabled(KEY_REPEAT);
    }

    void onRecordFinished() {
        // Note: Called from PA thread
        cout << "onRecordFinished" << endl;
        AudioClient *audio = sb::getAudioClient();
        cout << "Active: " << boolalpha << audio->isStreamActive() << endl;
        cout << "Stopped: " << boolalpha << audio->isStreamStopped() << endl;
        sb::getBot()->interpret(audio->data());
    }

    void pollInput() {
        sf::Event event;
        AudioClient *audio = sb::getAudioClient();
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                    audio->setCaptureCallback(onRecordFinished);
                    audio->record(MAX_SECONDS);
                    break;
                case sf::Event::MouseButtonReleased:
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

    sf::RenderWindow* getWindow() {
        return &window;
    }

    void setFullScreen(bool fs) {
        fullScreen = fs;
        if (fullScreen) {
            videoMode = sf::VideoMode::getFullscreenModes()[0];
        } else {
            videoMode = DEFAULT_VIDEO_MODE;
        }
    }

}
