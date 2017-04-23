#include "speech_api.h"
#include "base64.h"
#include <curl/curl.h>

#define SPEECH_API_URL          "https://speech.googleapis.com/v1/speech:recognize?key=AIzaSyCbVXMMo2EhmUPBl3Ikg-T9WgX20dshg4Q"
#define HEADER_ACCEPT           "Accept: application/json"
#define HEADER_CONTENT_TYPE     "Content-Type: application/json"
#define HEADER_CHARSETS         "charsets: utf-8"

// Reference: https://curl.haxx.se/libcurl/c/

namespace sb {

    static const json REQUEST_BODY = {
        {"config", {
            {"encoding", ENCODING},
            {"sampleRateHertz", SAMPLE_RATE},
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

    static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp);
    static bool curlAbort(CURL *curl, Response &response, char *buffer, bool result);

    bool speech2text(const AudioData *data, json &result) {
        CURL                *curl           = NULL;
        curl_slist          *requestHeaders = NULL;
        json                requestBody;
        const char          *requestBodyRaw;
        CURLcode            responseCode;
        Response            response;
        size_t              numSamples      = (size_t) (data->maxFrameIndex * NUM_CHANNELS);
        size_t              numBytes        = numSamples * sizeof(Sample);
        char                *buffer         = (char*) malloc(numBytes);
        string              encodedAudio;

        cout << "Sending new Voice API request" << endl;
        cout << "- Frames: " << data->maxFrameIndex << endl;


        // allocate space for response
        response.data = (char*) malloc(1);
        response.size = 0;
        if (response.data == NULL) {
            cerr << "Could not allocate memory for response" << endl;
            return curlAbort(curl, response, buffer, false);
        }

        // copy sample data
        memcpy(buffer, data->recordedSamples, numBytes);

        // encode data
        encodedAudio = base64_encode((const unsigned char*) buffer, (unsigned int) numBytes);
        requestBody = REQUEST_BODY;
        requestBody["audio"]["content"] = encodedAudio;
        requestBodyRaw = requestBody.dump().c_str();

        // construct headers
        const char *contentLength = ("Content-Length: " + to_string(strlen(requestBodyRaw))).c_str();
        requestHeaders = curl_slist_append(requestHeaders, HEADER_ACCEPT);
        requestHeaders = curl_slist_append(requestHeaders, HEADER_CONTENT_TYPE);
        requestHeaders = curl_slist_append(requestHeaders, contentLength);
        requestHeaders = curl_slist_append(requestHeaders, HEADER_CHARSETS);

        // initialize curl
        curl = curl_easy_init();
        if (curl) {
            // request properties
            curl_easy_setopt(curl, CURLOPT_URL, SPEECH_API_URL);
            curl_easy_setopt(curl, CURLOPT_POST, true);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, requestHeaders);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBodyRaw);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

            // send request
            responseCode = curl_easy_perform(curl);
            if (responseCode == CURLE_OK) {
                char *contentType;
                responseCode = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);
                if (responseCode == CURLE_OK && string(contentType) == "application/json; charset=UTF-8") {
                    result = json::parse(response.data);
                    return curlAbort(curl, response, buffer, true);
                } else {
                    cerr << "Invalid content type: " << contentType << endl;
                    return curlAbort(curl, response, buffer, false);
                }
            } else {
                cerr << "cURL responded with error (code " << responseCode << ")" << endl;
                return curlAbort(curl, response, buffer, false);
            }
        } else {
            cerr << "Could not initialize cURL" << endl;
            return curlAbort(curl, response, buffer, false);
        }
    }

    static bool curlAbort(CURL *curl, Response &response, char *buffer, bool result) {
        if (curl) {
            curl_easy_cleanup(curl);
        }
        if (response.data) {
            free(response.data);
        }
        if (buffer) {
            free(buffer);
        }
        cout << "cURL Aborted." << endl;
        return result;
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
