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
#include "sb.h"
#include "Shakebot.h"
#include "net/RestClient.h"
#include "cmd/Command.h"

using namespace sb;

static const Vector2f DEFAULT_SCALE (1, 1);
static const Vector2f DEFAULT_OFFSET(0, 0);

static const int ERR_LOAD_LANG  = 1;
static const int ERR_INIT_AUDIO = 2;

static Shakebot     *bot    = NULL;
static AudioClient  *audio  = NULL;
static bool         running = true;

/**
 * Starts the program in "wiki" mode where you can look up wikipedia articles.
 *
 * @return exit code
 */
static int startWikiMode();

int main(int argc, char *argv[]) {
    srand((unsigned) time(0)); // TODO: replace with better random

    // parse program arguments
    vector<string> args(argv, argv + argc);
    Vector2f scale = DEFAULT_SCALE;
    Vector2f offset = DEFAULT_OFFSET;
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "--test") {
            return runTests();
        } else if (args[i] == "--fullscreen") {
            setFullScreen(true);
            cout << "fullscreen=true" << endl;
        } else if (args[i] == "--scale") {
            scale = Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "scale=(" << scale.x << "," << scale.y << ")" << endl;
        } else if (args[i] == "--move") {
            offset = Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "offset=(" << offset.x << "," << offset.y << ")" << endl;
        } else if (args[i] == "--wiki") {
            return startWikiMode();
        }
    }

    // initialize
    cout << "Starting..." << endl;

    // initialize bot
    bot = new Shakebot();

    initWindow();
    RenderShakebot *render = bot->getRender();
    render->scale(scale);
    render->move(offset);

    startSpeech();

    if (!Command::loadLanguage()) {
        cerr << "Could not load voice commands" << endl;
        return ERR_LOAD_LANG;
    }

    audio = initAudio();
    if (audio == NULL) {
        cerr << "Could not initialize audio client" << endl;
        return ERR_INIT_AUDIO;
    }

    cout << "[running]" << endl;

    // main loop
    RenderWindow *window = getWindow();
    while (running) {
        pollInput();
        bot->update();
        render->draw(window);
        display();
    }

    // shutdown
    cout << "Shutting down..." << endl;
    stopSpeech();
    cout << "Goodbye." << endl;

    return 0;
}

int startWikiMode() {
    string query;
    while (running) {
        cout << "Enter a Wikipedia Page" << endl;
        cout << ">> ";
        getline(cin, query);
        cout << "Looking for " << query << endl;
        json result;
        if (!getWikiInfo(query, result)) {
            cerr << "Could not get results" << endl;
        } else {
            cout << result.dump(4) << endl;
        }
    }
    return 0;
}

namespace sb {

    Shakebot* getBot() {
        return bot;
    }

    AudioClient* getAudioClient() {
        return audio;
    }

    bool isRunning() {
        return running;
    }

    void setRunning(bool r) {
        running = r;
    }

    bool speech2text(const AudioData *data, json &result) {
        SpeechRequest request(data);

        // initialize client
        RestClient client;
        client.addHeader(HEADER_ACCEPT_JSON);
        client.addHeader(HEADER_CONTENT_TYPE_JSON);
        client.addHeader(HEADER_CHARSETS_UTF_8);
        client.addHeader("Content-Length: " + to_string(strlen(request.requestBody.dump().c_str())));
        if (!client.init()) {
            cerr << "Could not initialize REST client" << endl;
            return false;
        }

        // perform request
        cout << "Sending new Voice API request" << endl;
        cout << "- Frames: " << data->frameCount << endl;
        RestResponse *response = client.post(request.requestBody.dump(), SPEECH_API_URL);
        if (response == NULL) {
            cerr << "Could not perform POST request" << endl;
            return false;
        }

        result = response->asJson();

        return true;
    }

    bool getWikiInfo(const string &subject, json &result) {
        // initialize client
        RestClient client;
        client.addHeader(HEADER_ACCEPT_JSON);
        client.addHeader(HEADER_CONTENT_TYPE_JSON);
        client.addHeader(HEADER_CHARSETS_UTF_8);
        client.addHeader(HEADER_API_USER_AGENT);
        if (!client.init()) {
            cerr << "Could not initialize REST client" << endl;
            return false;
        }

        // perform request
        cout << "Sending new Wikipedia request" << endl;
        cout << "- Subject: " << subject << endl;

        size_t len = strlen(WIKI_API_URL);
        char url[len];
        sprintf(url, WIKI_API_URL, curl_easy_escape(client.getCurl(), subject.c_str(), (int) subject.size()));

        RestResponse *response = client.get(url, "application/json; charset=utf-8");
        if (response == NULL) {
            cerr << "Could not perform GET request" << endl;
            return false;
        }

        result = response->asJson();

        return true;
    }

}
