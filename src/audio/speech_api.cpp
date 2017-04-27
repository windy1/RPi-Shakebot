#include "speech_api.h"
#include "base64.h"
#include "../RestClient.h"

#define SPEECH_API_URL "https://speech.googleapis.com/v1/speech:recognize?key=AIzaSyCbVXMMo2EhmUPBl3Ikg-T9WgX20dshg4Q"

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
        // copy sample data -- TODO: necessary?
        size_t numSamples = (size_t) (data->frameCount * CHANNEL_COUNT_CAPTURE);
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
        client.addHeader(HEADER_ACCEPT_JSON);
        client.addHeader(HEADER_CONTENT_TYPE_JSON);
        client.addHeader(HEADER_CHARSETS_UTF_8);
        client.addHeader("Content-Length: " + to_string(strlen(requestBody.dump().c_str())));
        if (!client.init()) {
            cerr << "Could not initialize REST client" << endl;
            free(buffer);
            return false;
        }

        // perform request
        cout << "Sending new Voice API request" << endl;
        cout << "- Frames: " << data->frameCount << endl;
        RestResponse *response = client.post(requestBody.dump(), SPEECH_API_URL);
        if (response == NULL) {
            cerr << "Could not perform POST request" << endl;
            free(buffer);
            return false;
        }

        free(buffer);
        result = response->asJson();

        return true;
    }

}
