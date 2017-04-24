#ifndef SHAKESPEARE_CURL_UTILS_H
#define SHAKESPEARE_CURL_UTILS_H

#include <curl/curl.h>
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

namespace sb {

    struct Response {

        CURLcode    code;
        char        *data = NULL;
        size_t      size;

        json json();

    };

    class RestClient {

        static const string DEFAULT_RESULT_TYPE;

        CURL        *curl           = NULL;
        curl_slist  *requestHeaders = NULL;
        bool        verbose         = true;
        Response    response;

        bool perform(string url, bool post, string resultType = DEFAULT_RESULT_TYPE);

    public:

        ~RestClient();

        bool isVerbose() const;

        void setVerbose(bool verbose);

        void addHeader(string header);

        bool init();

        bool post(string data, string url, string resultType = DEFAULT_RESULT_TYPE);

        bool get(string url, string resultType = DEFAULT_RESULT_TYPE);

        Response* getResponse();

    };

}

#endif //SHAKESPEARE_CURL_UTILS_H
