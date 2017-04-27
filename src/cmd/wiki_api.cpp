#include "wiki_api.h"
#include "../RestClient.h"

#define HEADER_API_USER_AGENT "User-Agent: wcrouse@uvm.edu"

using namespace std;

namespace sb {

    static const char *WIKIPEDIA_API_URL = "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&exintro=&explaintext=&titles=%s";

    bool getWikiInfo(string subject, json &result) {
        // initialize client
        RestClient client;
        client.addHeader(HEADER_ACCEPT_JSON);
        client.addHeader(HEADER_CONTENT_TYPE_JSON);
        client.addHeader(HEADER_CHARSETS_UTF_8);
        client.addHeader(HEADER_API_USER_AGENT);
        if (!client.init()) {
            cerr << "Could not initialize REST client" << endl;
            return false;
        }

        // perform request
        cout << "Sending new Wikipedia request" << endl;
        cout << "- Subject: " << subject << endl;

        size_t len = strlen(WIKIPEDIA_API_URL);
        char url[len];
        sprintf(url, WIKIPEDIA_API_URL, curl_easy_escape(client.getCurl(), subject.c_str(), (int) subject.size()));

        RestResponse *response = client.get(url, "application/json; charset=utf-8");
        if (response == NULL) {
            cerr << "Could not perform GET request" << endl;
            return false;
        }

        result = response->asJson();

        return true;
    }

}
