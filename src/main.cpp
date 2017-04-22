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
    sf::Vector2f offset(0, 0);
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
            cout << "fullscreen=true" << endl;
        } else if (args[i] == "--scale") {
            scale = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "scale=(" << scale.x << "," << scale.y << ")" << endl;
        } else if (args[i] == "--move") {
            offset = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "offset=(" << offset.x << "," << offset.y << ")" << endl;
        }
    }
    cout << "Starting..." << endl;
    graphics.init();
    graphics.getBotRender()->scale(scale);
    graphics.getBotRender()->move(offset);
    sb::startSpeech();
    cout << "[running]" << endl;
    while (running) {
        graphics.clear();
        graphics.pollInput();
        bot.update();
        graphics.push();
    }
    cout << "Shutting down..." << endl;
    sb::stopSpeech();
    cout << "Goodbye." << endl;
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
