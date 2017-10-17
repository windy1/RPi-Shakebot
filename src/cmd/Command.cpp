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
#include "Command.h"
#include "GreetingCommand.h"
#include "WikiCommand.h"
#include <fstream>

namespace sb {

    const string Command::GREETING  = "greeting";
    const string Command::WIKI      = "wiki";

    /// Token phrases to identify commands
    static vector<string> greetingQualifiers;
    static vector<string> greetingResponses;
    static vector<string> wikiQualifiers;

    static bool readLines(const string &fileName, vector<string> &buffer);

    cmd_ptr Command::parse(string raw) {
        for (const string &qualifier : wikiQualifiers) {
            if (raw.substr(0, qualifier.size()) == qualifier) {
                string subject = raw.substr(qualifier.size() + 1);
                return make_shared<WikiCommand>(subject);
            }
        }
        for (const string &qualifier : greetingQualifiers) {
            if (raw.substr(0, qualifier.size()) == qualifier) {
                // command is a greeting
                return make_shared<GreetingCommand>();
            }
        }
        return NULL;
    }

    const vector<string>& Command::getGreetingResponses() {
        return greetingResponses;
    }

    bool Command::loadLanguage() {
        cout << "Loading commands" << endl;
        bool success = true;
        success &= readLines("greetings.txt", greetingResponses);
        success &= readLines("greetings_q.txt", greetingQualifiers);
        success &= readLines("wiki_q.txt", wikiQualifiers);
        return success;
    }

    bool readLines(const string &fileName, vector<string> &buffer) {
        cout << "- " << fileName << endl;
        ifstream file(fileName);
        if (!file) {
            cerr << "Failed to open " << fileName << endl;
            return false;
        }
        string ln;
        while (getline(file, ln) && !ln.empty()) {
            buffer.push_back(ln);
        }
        file.close();
        return true;
    }

}
