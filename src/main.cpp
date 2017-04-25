#include <iostream>
#include "app.h"
#include "tests.h"
#include "audio/speech.h"
#include "Shakebot.h"
#include "graphics/graphics.h"
#include "cmd/Command.h"

sb::Shakebot    bot;
sb::AudioClient *audio  = NULL;
bool            running = true;

int main(int argc, char *argv[]) {
    srand((unsigned) time(NULL));
    // parse arguments
    vector<string> args(argv, argv + argc);
    sf::Vector2f scale(1, 1);
    sf::Vector2f offset(0, 0);
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "--test") {
            return sb::runTests();
        } else if (args[i] == "--fullscreen") {
            sb::setFullScreen(true);
            cout << "fullscreen=true" << endl;
        } else if (args[i] == "--scale") {
            scale = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "scale=(" << scale.x << "," << scale.y << ")" << endl;
        } else if (args[i] == "--move") {
            offset = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "offset=(" << offset.x << "," << offset.y << ")" << endl;
        }
    }

    // initialize
    cout << "Starting..." << endl;
    sb::initWindow();
    sb::RenderShakebot *render = bot.getRender();
    render->scale(scale);
    render->move(offset);

    sb::startSpeech();

    sb::Command::loadLanguage();

    audio = sb::initAudio();
    if (audio == NULL) {
        cerr << "Could not initialize audio client" << endl;
        return 1;
    }

    cout << "[running]" << endl;

    sf::RenderWindow *window = sb::getWindow();
    while (running) {
        sb::pollInput();
        bot.update();
        render->draw(window);
        sb::display();
    }

    // shutdown
    cout << "Shutting down..." << endl;
    sb::stopSpeech();
    cout << "Goodbye." << endl;

    return 0;
}

namespace sb {

    Shakebot* getBot() {
        return &bot;
    }

    AudioClient* getAudioClient() {
        return audio;
    }

    bool isRunning() {
        return running;
    }

    void setRunning(bool r) {
        running = r;
    }

}
