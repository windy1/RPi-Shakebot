#ifndef SHAKESPEARE_WIKI_API_H
#define SHAKESPEARE_WIKI_API_H

#include <string>
#include <json.hpp>

using json = nlohmann::json;
using namespace std;

namespace sb {

    bool getWikiInfo(string subject, json &result);

}

#endif //SHAKESPEARE_WIKI_API_H
