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
