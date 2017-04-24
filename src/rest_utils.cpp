#include <cstdlib>
#include "rest_utils.h"

namespace sb {

    const string RestClient::DEFAULT_RESULT_TYPE = "application/json; charset=UTF-8";

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp);

    RestClient::~RestClient() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
        if (response.data) {
            free(response.data);
        }
    }

    bool RestClient::isVerbose() const {
        return verbose;
    }

    void RestClient::setVerbose(bool verbose) {
        this->verbose = verbose;
    }

    void RestClient::addHeader(string header) {
        requestHeaders = curl_slist_append(requestHeaders, header.c_str());
    }

    bool RestClient::init() {
        if (response.data) {
            cerr << "Response data already allocated" << endl;
            return false;
        }
        if (curl) {
            cerr << "cURL already initialized" << endl;
            return false;
        }

        response.data = (char*) malloc(1);
        response.size = 0;
        if (response.data == NULL) {
            cerr << "Could not allocate memory for response" << endl;
            return false;
        }

        curl = curl_easy_init();
        if (!curl) {
            cerr << "Could not initialize cURL" << endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, requestHeaders);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose);

        return true;
    }

    bool RestClient::post(string data, string url, string resultType) {
        assert(curl != NULL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        return perform(url, true, resultType);
    }

    bool RestClient::get(string url, string resultType) {
        return perform(url, false, resultType);
    }

    bool RestClient::perform(string url, bool post, string resultType) {
        // perform curl
        assert(curl != NULL);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, post);
        response.code = curl_easy_perform(curl);
        if (response.code != CURLE_OK) {
            cerr << "cURL responded with an error (code " << response.code << ")" << endl;
            return false;
        }

        // check result type
        char *contentType;
        response.code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);
        if (response.code != CURLE_OK) {
            cerr << "Could not get content type" << endl;
            return false;
        }
        if (string(contentType) != resultType) {
            cerr << "Unexpected result content type: " << contentType << endl;
            return false;
        }

        return true;
    }

    Response* RestClient::getResponse() {
        return &response;
    }

    json Response::json() {
        return json::parse(data);
    }

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t realSize = size * nmemb;
        Response *response = (Response*) userp;
        response->data = (char*) realloc(response->data, response->size + realSize + 1);
        if(response->data == NULL) {
            cerr << "Error: Could not reallocate response buffer" << endl;
            return 0;
        }
        memcpy(&(response->data[response->size]), contents, realSize);
        response->size += realSize;
        response->data[response->size] = 0;
        return realSize;
    }

}
