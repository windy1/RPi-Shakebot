#include <iostream>
#include "app.h"
#include "graphics/Graphics.h"
#include "tests.h"
#include "audio/speech.h"

sb::Graphics graphics;
sb::Shakebot bot;
bool running = true;

int main(int argv, char *argc[]) {
    if (argv > 1 && string(argc[1]) == "--test") {
        cout << "Running tests..." << endl;
        int failed = 0;
        //failed += sb::testCountSyllables();
        //failed += sb::testFestival();
        failed += sb::testPortAudio();
        return failed;
    }
    graphics.setFullScreen(string(argc[1]) == "--fullscreen");
    cout << "Starting..." << endl;
    graphics.init();
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
