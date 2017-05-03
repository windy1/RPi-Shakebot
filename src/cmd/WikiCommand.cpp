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
#include "WikiCommand.h"

namespace sb {

    WikiCommand::WikiCommand(string subject) : subject(subject) {
    }

    string WikiCommand::getType() const {
        return WIKI;
    }

    string WikiCommand::execute() {
        json result;
        if (!getWikiInfo(subject, result)) {
            cerr << "Could not get Wikipedia results" << endl;
            return "";
        }
        if (result.empty() || result.find("query") == result.end()) {
            return "";
        }
        json query = result["query"];
        if (query.find("pages") == query.end()) {
            return "";
        }
        json firstPage = *query["pages"].begin();
        if (firstPage.find("extract") == firstPage.end()) {
            return "";
        }
        string extract = firstPage["extract"];
        if (extract.size() > MAX_RESULT_SIZE) {
            extract = extract.substr(0, MAX_RESULT_SIZE);
            size_t lastStop = extract.find_last_of('.');
            if (lastStop != string::npos) {
                extract = extract.substr(0, lastStop);
            }
        }
        return extract;
    }

}
