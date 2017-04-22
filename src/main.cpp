#include <iostream>
#include "app.h"
#include "graphics/Graphics.h"
#include "tests.h"
#include "audio/speech.h"

sb::Graphics graphics;
sb::Shakebot bot;
bool running = true;

int main(int argc, char *argv[]) {
    vector<string> args(argv, argv + argc);
    sf::Vector2f scale(1, 1);
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "--test") {
            cout << "Running tests..." << endl;
            int failed = 0;
            //failed += sb::testCountSyllables();
            //failed += sb::testFestival();
            failed += sb::testPortAudio();
            return failed;
        } else if (args[i] == "--fullscreen") {
            graphics.setFullScreen(true);
        } else if (args[i] == "--scale") {
            scale = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
        }
    }
    cout << "Starting..." << endl;
    graphics.init();
    graphics.getBotRender()->scale(scale);
    sb::startSpeech();
    while (running) {
        graphics.clear();
        graphics.pollInput();
        bot.update();
        graphics.push();
    }
    sb::stopSpeech();
    return 0;
}

sb::Shakebot* sb::getBot() {
    return &bot;
}

bool sb::isRunning() {
    return running;
}

void sb::setRunning(bool r) {
    running = r;
}
