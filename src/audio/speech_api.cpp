#include "speech_api.h"
#include "base64.h"
#include <curl/curl.h>

#define SPEECH_API_URL "https://speech.googleapis.com/v1beta1/speech:syncrecognize?key=AIzaSyCbVXMMo2EhmUPBl3Ikg-T9WgX20dshg4Q"

// Reference: https://curl.haxx.se/libcurl/c/

namespace sb {

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp);

    static const json REQUEST_BODY = {
        {"config", {
            {"encoding", ENCODING},
            {"sampleRate", SAMPLE_RATE},
            {"languageCode", LANGUAGE_CODE}
        }},
        {"audio", {
            {"content", ""}
        }}
    };

    struct Response {
        char *data;
        size_t size;
    };

    json speech2text(const AudioData &data) {
        // this implementation will work for single-byte samples only
        assert(sizeof(Sample) == 1);

        Response response;
        response.data = (char*) malloc(1);
        response.size = 0;
        if (response.data == NULL) {
            cout << "Error: Could not allocate memory for response" << endl;
            return {};
        }

        // build request body
        unsigned int numSamples = (unsigned int) (data.maxFrameIndex * NUM_CHANNELS);
        vector<uint8_t> buffer(numSamples);
        for (int i = 0; i < numSamples; i++) {
            buffer[i] = data.recordedSamples[i];
        }
        string encoded = base64_encode((const unsigned char*) &buffer[0], numSamples);
        json body = REQUEST_BODY;
        body["audio"]["content"] = encoded;
        const char *bodyData = body.dump().c_str();

        // construct headers
        struct curl_slist *headers = NULL;
        string len = "Content-Length: " + to_string(strlen(bodyData));
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, len.c_str());
        headers = curl_slist_append(headers, "charsets: utf-8");

        // initialize curl
        CURL *curl;
        CURLcode responseCode;
        curl = curl_easy_init();
        if (curl) {
            // request properties
            curl_easy_setopt(curl, CURLOPT_URL, SPEECH_API_URL);
            curl_easy_setopt(curl, CURLOPT_POST, true);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyData);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
            // send request
            responseCode = curl_easy_perform(curl);
            if (responseCode == CURLE_OK) {
                char *ct;
                responseCode = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
                if (responseCode == CURLE_OK && ct) {
                    return json::parse(response.data);
                }
            }
        }
        return {};
    }

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t realSize = size * nmemb;
        Response *response = (Response*) userp;
        response->data = (char*) realloc(response->data, response->size + realSize + 1);
        if(response->data == NULL) {
            cout << "Error: Could not reallocate response buffer" << endl;
            return 0;
        }
        memcpy(&(response->data[response->size]), contents, realSize);
        response->size += realSize;
        response->data[response->size] = 0;
        return realSize;
    }

}
