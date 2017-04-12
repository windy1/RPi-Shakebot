#include "Shakebot.h"
#include <regex>
#include <iostream>
#include <festival/festival.h>
#include "audio/speech.h"

const regex syllableRegex("[aeiouy]+");

namespace sb {

    void Shakebot::say(const string phrase) {
        phraseBuffer = phrase;
    }

    void Shakebot::update() {
        if (joinVox) {
            if (voxFuture.valid()) {
                voxFuture.get();
            }
            joinVox = false;
        }
        if (!phraseBuffer.empty() || !currentWord.empty()) {
            if (currentWord.empty()) {
                if (firstWord) {
                    if (!voxStarted) {
                        voxPromise = promise<bool>();
                        sb::pushSpeech(phraseBuffer, voxPromise);
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
                isMouthOpened = wordUpdates % 2 == 0;
                if (!isMouthOpened && ++syllableCount >= wordSyllables) {
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

    int countSyllables(const string phrase, unsigned int n) {
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
