#include "voice.h"
#include <queue>
#include <festival/festival.h>
#include "json.hpp"
#include "base64.h"
#include <curl/curl.h>

#define SPEECH_API_URL "https://speech.googleapis.com/v1beta1/speech:syncrecognize"

using namespace std;
using json = nlohmann::json;

namespace sb {

    static bool running = false;
    static queue<Phrase> toSay;
    static string *responseBody;

    static int writer(char *data, size_t size, size_t nmemb, std::string *buffer_in);

    void run() {
        running = true;
        festival_initialize(true, 210000);
        while (running) {
            if (!toSay.empty()) {
                Phrase phrase = toSay.front();
                toSay.pop();
                if (!festival_say_text(phrase.str.data())) {
                    cout << "Error: Could not say text" << endl;
                }
                phrase.pr.set_value();
            }
        }
    }

    thread startVoiceThread() {
        return thread(run);
    }

    void stopVoiceThread() {
        running = false;
    }

    void toVoice(const string str, promise<void> &pr) {
        toSay.push({pr, str});
    }

    string toText(const AudioData &data) {
        // build request body
        unsigned int numSamples = (unsigned int) (data.maxFrameIndex * NUM_CHANNELS);
        vector<uint8_t> buffer(numSamples);
        for (int i = 0; i < numSamples; i++) {
            buffer[i] = data.recordedSamples[i];
        }
        string encoded = base64_encode((const unsigned char*) &buffer[0], numSamples);
        json body = {
          {"config", {
             {"encoding", ENCODING},
             {"sampleRate", SAMPLE_RATE},
             {"languageCode", LANGUAGE_CODE}
          }},
          {"audio", {
             {"content", encoded}
          }}
        };

        // send speech api request
        cout << "sending request..." << endl;
        CURL *curl;
        CURLcode response;
        struct curl_slist *headers = NULL;
        string len = "Content-Length: " + to_string(body.size());
        cout << len.c_str() << endl;
        curl_slist_append(headers, "Accept: application/json");
        curl_slist_append(headers, "Content-Type: application/json");
        curl_slist_append(headers, len.c_str());
        curl_slist_append(headers, "Authorization: Bearer ya29.El8rBAvlJa0Hc5cK_vzou-gjTl3-9b40QtekEphhReOVM-lj1o00Rcq5BhgM2BSDtIwFfIsJO12VHkdnjbjJSnOvDNJ7U1GOeqZUIyaGm6JOHz6rGEBg0lLeGJKuZ7NTig");
        curl_slist_append(headers, "charsets: utf-8");
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl, CURLOPT_URL, SPEECH_API_URL);
            curl_easy_setopt(curl, CURLOPT_POST, true);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.dump().c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
            response = curl_easy_perform(curl);
            if (response == CURLE_OK) {
                char *ct;
                response = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
                cout << "ct = " << ct << endl;
                if (response == CURLE_OK && ct) {
                    cout << *responseBody << endl;
                }
            }
        }
        return "";
    }

    static int writer(char *data, size_t size, size_t nmemb, std::string *bufferIn) {
        if (bufferIn != NULL) {
            bufferIn->append(data, size * nmemb);
            responseBody = bufferIn;
            return (int) (size * nmemb);
        }
        return 0;
    }

}
