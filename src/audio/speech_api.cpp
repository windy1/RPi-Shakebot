#include "speech_api.h"
#include "base64.h"
#include "../rest_utils.h"
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

    bool speech2text(const AudioData *data, json &result) {
        // copy sample data
        size_t numSamples = (size_t) (data->maxFrameIndex * NUM_CHANNELS);
        size_t numBytes = numSamples * sizeof(Sample);
        char *buffer = (char*) malloc(numBytes);
        if (buffer == NULL) {
            cerr << "Could not allocate request buffer" << endl;
            return false;
        }
        memcpy(buffer, data->recordedSamples, numBytes);

        // encode data for request
        string encodedAudio = base64_encode((const unsigned char*) buffer, (unsigned int) numBytes);
        json requestBody = REQUEST_BODY;
        requestBody["audio"]["content"] = encodedAudio;

        // initialize client
        RestClient client;
        client.addHeader(HEADER_ACCEPT);
        client.addHeader(HEADER_CONTENT_TYPE);
        client.addHeader("Content-Length: " + to_string(strlen(requestBody.dump().c_str())));
        client.addHeader(HEADER_CHARSETS);
        if (!client.init()) {
            cerr << "Could not initialize REST client" << endl;
            free(buffer);
            return false;
        }

        // perform request
        cout << "Sending new Voice API request" << endl;
        cout << "- Frames: " << data->maxFrameIndex << endl;
        if (!client.post(requestBody.dump(), SPEECH_API_URL)) {
            cerr << "Could not perform POST request" << endl;
            free(buffer);
            return false;
        }

        free(buffer);
        result = client.getResponse()->json();

        return true;
    }

}
