#include "Shakebot.h"
#include <regex>
#include <iostream>
#include "audio/speech.h"
#include "cmd/Command.h"

const regex syllableRegex("[aeiouy]+");

namespace sb {

    Shakebot::Shakebot() : render(RenderShakebot(this, "images/shakespeare.jpg", sf::IntRect(166, 243, 54, 65))) {
    }

    bool Shakebot::isMouthOpened() {
        return mouthOpened;
    }

    RenderShakebot* Shakebot::getRender() {
        return &render;
    }

    void Shakebot::say(string phrase) {
        phraseBuffer = phrase;
    }

    void Shakebot::update() {
        if (joinVox) {
            if (voxFuture.valid()) {
                if (!voxFuture.get()) {
                    cerr << "Error: Could not say last phrase." << endl;
                }
            }
            joinVox = false;
        }
        if (!phraseBuffer.empty() || !currentWord.empty()) {
            if (currentWord.empty()) {
                if (firstWord) {
                    if (!voxStarted) {
                        voxPromise = promise<bool>();
                        pushSpeech(phraseBuffer, voxPromise);
                        voxFuture = voxPromise.get_future();
                        delayClock.restart();
                        voxStarted = true;
                    }
                    if (delayClock.getElapsedTime() < delayTime) {
                        return;
                    }
                }
                // take next word from buffer
                unsigned long wordBreak = phraseBuffer.find(" ");
                if (wordBreak != string::npos) {
                    currentWord = phraseBuffer.substr(0, wordBreak);
                    phraseBuffer = phraseBuffer.substr(wordBreak + 1);
                } else {
                    currentWord = phraseBuffer;
                    phraseBuffer.clear();
                }
                wordSyllables = countSyllables(currentWord);
                firstWord = false;
            }
            if (pauseClock.getElapsedTime() >= pauseTime) {
                mouthOpened = wordUpdates % 2 == 0;
                if (!mouthOpened && ++syllableCount >= wordSyllables) {
                    // word complete
                    currentWord.clear();
                    wordSyllables = 0;
                    wordUpdates = 0;
                    syllableCount = 0;
                    if (phraseBuffer.empty()) {
                        firstWord = true;
                        voxStarted = false;
                        joinVox = true;
                    }
                } else {
                    wordUpdates++;
                }
                pauseClock.restart();
            }
        }
    }

    bool Shakebot::interpret(const AudioData *data) {
        json response;
        if (!speech2text(data, response)) {
            cerr << "Could not retrieve voice translation" << endl;
            return false;
        }
        cout << response.dump(4) << endl;
        if (response.empty()) {
            cerr << "Empty response" << endl;
            return false;
        }

        json alternatives = response["results"][0]["alternatives"];
        cout << alternatives << endl;
        for (auto alt : alternatives) {
            cout << alt << endl;
            cmd_ptr cmd = Command::parse(alt["transcript"]);
            if (cmd != NULL) {
                say(cmd->execute());
            }
        }

        return true;
    }

    int countSyllables(string phrase, unsigned int n) {
        if (phrase.empty()) {
            return n;
        }
        string word = phrase;
        unsigned long wordBreak = phrase.find(" ");
        if (wordBreak != string::npos) {
            word = phrase.substr(0, wordBreak);
        }
        unsigned int len = (unsigned int) word.size();
        if (word[len - 1] == 'e') {
            // remove trailing Es
            word = word.substr(0, len - 1);
        } else if (len > 3 && word.substr(len - 2) == "ed" && word[len - 3] != 't' && word[len - 3] != 'd') {
            word = word.substr(0, len - 2);
        }
        auto begin = sregex_iterator(word.begin(), word.end(), syllableRegex);
        auto end = sregex_iterator();
        if (wordBreak == string::npos) {
            return n + max((int) distance(begin, end), 1);
        }
        return countSyllables(phrase.substr(wordBreak + 1), n + max((int) distance(begin, end), 1));
    }

    int countSyllables(string phrase) {
        return countSyllables(phrase, 0);
    }

}
