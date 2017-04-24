#include <cstdlib>
#include "RestClient.h"

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
        // preconditions
        if (response.data) {
            cerr << "Response data already allocated" << endl;
            return false;
        }
        if (curl) {
            cerr << "cURL already initialized" << endl;
            return false;
        }

        // allocate response
        response.data = (char*) malloc(1);
        response.size = 0;
        if (response.data == NULL) {
            cerr << "Could not allocate memory for response" << endl;
            return false;
        }

        // initialize curl
        curl = curl_easy_init();
        if (curl == NULL) {
            cerr << "Could not initialize cURL" << endl;
            return false;
        }

        // configure curl
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, requestHeaders);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose);

        return true;
    }

    RestResponse* RestClient::post(string data, string url, string resultType) {
        assert(curl != NULL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        return perform(url, true, resultType);
    }

    RestResponse* RestClient::get(string url, string resultType) {
        return perform(url, false, resultType);
    }

    RestResponse* RestClient::perform(string url, bool post, string resultType) {
        assert(curl != NULL);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, post);

        // perform curl
        response.code = curl_easy_perform(curl);
        if (response.code != CURLE_OK) {
            cerr << "cURL responded with an error (code " << response.code << ")" << endl;
            return NULL;
        }

        // check result type
        char *contentType;
        response.code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);
        if (response.code != CURLE_OK) {
            cerr << "Could not get content type" << endl;
            return NULL;
        }
        if (string(contentType) != resultType) {
            cerr << "Unexpected result content type: " << contentType << endl;
            return NULL;
        }

        return &response;
    }

    json RestResponse::json() {
        assert(data != NULL);
        return json::parse(data);
    }

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t realSize = size * nmemb;
        RestResponse *response = (RestResponse*) userp;
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
