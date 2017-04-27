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
        cout << "result = " << result << endl;
        if (result.empty()) {
            return "";
        }
        return (*result["query"]["pages"].begin())["extract"];
    }

}
