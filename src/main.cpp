#include "app.h"
#include "graphics/Graphics.h"
#include "Shakebot.h"
#include "tests.h"
#include "audio/voice.h"
#include <fstream>
#include <festival/festival.h>

sb::Graphics *graphics = new sb::Graphics();
sb::Shakebot *bot = new sb::Shakebot();
bool running = true;
thread voiceThread;

int main(int argv, char *argc[]) {
    if (argv > 1 && string(argc[1]) == "--test") {
        int failed = 0;
        //failed += sb::testCountSyllables();
        //failed += sb::testFestival();
        failed += sb::testPortAudio();
        return failed;
    }
    graphics->init();
    voiceThread = sb::vox::start();
    while (running) {
        graphics->clear();
        graphics->pollInput();
        bot->update();
        graphics->push();
    }
    sb::vox::stop();
    voiceThread.join();
    return 0;
}

sb::Shakebot* sb::getBot() {
    return bot;
}

bool sb::isRunning() {
    return running;
}

void sb::setRunning(bool r) {
    running = r;
}
