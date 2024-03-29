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
#include "sb.h"
#include "Shakebot.h"
#include "cmd/Command.h"
#include <regex>

const regex syllableRegex("[aeiouy]+");

namespace sb {

    Shakebot::Shakebot() : render(RenderShakebot(this, "images/shakespeare.jpg", IntRect(166, 243, 54, 65))) {
    }

    bool Shakebot::isMouthOpened() {
        return mouthOpened;
    }

    RenderShakebot* Shakebot::getRender() {
        return &render;
    }

    void Shakebot::say(const string &phrase) {
        cout << "Pushing phrase: " << phrase << endl;
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
                size_t wordBreak = phraseBuffer.find(' ');
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

        if (in != NULL) {
            json response;
            if (!speech2text(in, response)) {
                cerr << "Could not retrieve voice translation" << endl;
                in = NULL;
                return;
            }
            cout << response.dump(4) << endl;
            if (response.empty()) {
                cerr << "Empty response" << endl;
                in = NULL;
                return;
            }

            json alternatives = response["results"][0]["alternatives"];
            cout << alternatives << endl;
            for (auto alt : alternatives) {
                cout << alt << endl;
                cmd_ptr cmd = Command::parse(alt["transcript"]);
                if (cmd != NULL) {
                    string result = cmd->execute();
                    if (!result.empty()) {
                        say(result);
                    }
                }
            }

            in = NULL;
        }
    }

    void Shakebot::interpret(const AudioData *data) {
        in = data;
    }

    int countSyllables(const string &phrase, unsigned int n) {
        if (phrase.empty()) {
            return n;
        }
        string word = phrase;
        unsigned long wordBreak = phrase.find(' ');
        if (wordBreak != string::npos) {
            word = phrase.substr(0, wordBreak);
        }
        auto len = (unsigned int) word.size();
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

    int countSyllables(const string &phrase) {
        return countSyllables(phrase, 0);
    }

}
