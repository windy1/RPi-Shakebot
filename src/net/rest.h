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
#ifndef SHAKESPEARE_REST_H
#define SHAKESPEARE_REST_H

#define HEADER_ACCEPT_JSON          "Accept: application/json"
#define HEADER_CONTENT_TYPE_JSON    "Content-Type: application/json"
#define HEADER_CHARSETS_UTF_8       "charsets: utf-8"
#define HEADER_API_USER_AGENT       "User-Agent: wcrouse@uvm.edu"

#define SPEECH_API_URL "https://speech.googleapis.com/v1/speech:recognize?key=AIzaSyCbVXMMo2EhmUPBl3Ikg-T9WgX20dshg4Q"

#include "../sb.h"

namespace sb {

    /// A template for speech requests
    static const json SPEECH_REQUEST_BODY = {
        {"config", {
            {"encoding", ENCODING},
            {"sampleRateHertz", SAMPLE_RATE},
            {"languageCode", LANGUAGE_CODE}
        }},
        {"audio", {
            {"content", ""}
        }}
    };

    static const char *WIKI_API_URL =
        "https://en.wikipedia.org/w/api.php?"   /// main url
        "format=json"                           /// output type
        "&action=query"                         /// api action
        "&prop=extracts"                        /// only give plaintext
        "&exintro="                             /// give only intro
        "&explaintext="                         ///
        "&titles=%s";                           /// title

    /**
     * A request to the Speech API
     */
    struct SpeechRequest {

        json requestBody = SPEECH_REQUEST_BODY; /// json payload

        SpeechRequest(const AudioData *data) {
            // copy sample memory block to chars
            char *buffer = (char*) malloc(data->size());
            if (buffer == NULL) {
                cerr << "Could not allocate request buffer" << endl;
                return;
            }
            memcpy(buffer, data->recordedSamples, data->size());
            // encode base 64
            requestBody["audio"]["content"] = base64_encode((const unsigned char*) buffer, data->size());
            free(buffer);
        }

    };

}

#endif //SHAKESPEARE_REST_H
