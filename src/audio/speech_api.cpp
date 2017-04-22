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

    bool speech2text(const AudioData *data, json &result) {
        bool                success         = true;
        CURL                *curl           = 0;
        curl_slist          *requestHeaders = NULL;
        json                requestBody;
        const char          *requestBodyRaw;
        CURLcode            responseCode;
        Response            response;
        size_t              numSamples      = (size_t) (data->maxFrameIndex * NUM_CHANNELS);
        size_t              numBytes        = numSamples * sizeof(Sample);
        char                *buffer         = (char*) malloc(numBytes);
        string              encodedAudio;

        // allocate space for response
        response.data = (char*) malloc(1);
        response.size = 0;
        if (response.data == NULL) {
            cerr << "Error: Could not allocate memory for response" << endl;
            success = false;
            goto cleanup;
        }

        // copy sample data
        memcpy(buffer, data->recordedSamples, numBytes);

        // encode data
        encodedAudio = base64_encode((const unsigned char*) buffer, (unsigned int) numBytes);
        requestBody = REQUEST_BODY;
        requestBody["audio"]["content"] = encodedAudio;
        requestBodyRaw = requestBody.dump().c_str();

        // construct headers
        requestHeaders = curl_slist_append(requestHeaders, "Accept: application/json");
        requestHeaders = curl_slist_append(requestHeaders, "Content-Type: application/json");
        requestHeaders = curl_slist_append(requestHeaders,
                ("Content-Length: " + to_string(strlen(requestBodyRaw))).c_str());
        requestHeaders = curl_slist_append(requestHeaders, "charsets: utf-8");

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
                    goto cleanup;
                } else {
                    cerr << "Error: Invalid content type: " << contentType << endl;
                    success = false;
                    goto cleanup;
                }
            } else {
                cerr << "Error: cURL responded with error (code " << responseCode << ")" << endl;
                success = false;
                goto cleanup;
            }
        } else {
            cerr << "Error: Could not initialize cURL" << endl;
            success = false;
            goto cleanup;
        }

        cleanup:
        if (curl) {
            curl_easy_cleanup(curl);
        }
        if (response.data) {
            free(response.data);
        }
        if (buffer) {
            free(buffer);
        }
        return success;
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
