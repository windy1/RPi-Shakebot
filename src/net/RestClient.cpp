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
#include <cstdlib>
#include "RestClient.h"

namespace sb {

    const string RestClient::DEFAULT_RESULT_TYPE = "application/json; charset=UTF-8";

    /// libCURL callback
    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp);

    RestClient::~RestClient() {
        // cleanup
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    CURL* RestClient::getCurl() const {
        return curl;
    }

    bool RestClient::isVerbose() const {
        return verbose;
    }

    void RestClient::setVerbose(bool verbose) {
        this->verbose = verbose;
    }

    void RestClient::addHeader(string header) {
        assert(!header.empty());
        requestHeaders = curl_slist_append(requestHeaders, header.c_str());
    }

    bool RestClient::init() {
        if (response.data != NULL) {
            cerr << "Response data already allocated" << endl;
            return false;
        }
        if (curl != NULL) {
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

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, requestHeaders);     // add request headers
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);   // set response callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);           // set data object
        curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose);               // set verbose mode

        return true;
    }

    RestResponse* RestClient::post(string data, string url, string resultType) {
        assert(curl != NULL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()); // set post data
        return perform(url, true, resultType);
    }

    RestResponse* RestClient::get(string url, string resultType) {
        return perform(url, false, resultType);
    }

    RestResponse* RestClient::perform(string url, bool post, string resultType) {
        assert(curl != NULL);
        assert(!url.empty());

        if (verbose) {
            cout << "[============================================================ cURL ";
            cout << "============================================================]" << endl;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());   // set request url
        curl_easy_setopt(curl, CURLOPT_POST, post);         // set request method

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

        if (verbose) {
            cout << "[=====================================================================";
            cout << "=========================================================]" << endl;
        }

        return &response;
    }

    RestResponse::RestResponse() {
    }

    RestResponse::RestResponse(const RestResponse &response) {
        code = response.code;
        size = response.size;
        data = (char*) malloc(size);
        memcpy(data, response.data, size);
    }

    RestResponse::~RestResponse() {
        if (data != NULL) {
            free(data);
        }
    }

    bool RestResponse::empty() {
        return size == 0;
    }

    json RestResponse::asJson() {
        assert(data != NULL);
        if (empty()) {
            return json();
        }
        return json::parse(data);
    }

    ostream& operator<<(ostream &out, const RestResponse &response) {
        out << response.data << endl;
        return out;
    }

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp) {
        // reallocate data block while reading
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
