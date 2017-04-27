#include "SearchCommand.h"
#include "wiki_api.h"

namespace sb {

    SearchCommand::SearchCommand(string subject) : subject(subject) {
    }

    string SearchCommand::getType() const {
        return SEARCH;
    }

    string SearchCommand::execute() {
        json result;
        if (!getWikiInfo(subject, result)) {
            cerr << "Could not get Wikipedia results" << endl;
            return "";
        }
        if (result.empty()) {
            return "";
        }
        cout << result << endl;
        return (*result["query"]["pages"].begin())["extract"];
    }

}
