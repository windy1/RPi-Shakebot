#include "sb.h"
#include "Shakebot.h"
#include "cmd/Command.h"
#include "net/RestClient.h"

sb::Shakebot    bot;
sb::AudioClient *audio  = NULL;
bool            running = true;

int main(int argc, char *argv[]) {
    srand((unsigned) time(NULL));
    // parse arguments
    vector<string> args(argv, argv + argc);
    sf::Vector2f scale(1, 1);
    sf::Vector2f offset(0, 0);
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "--test") {
            return sb::runTests();
        } else if (args[i] == "--fullscreen") {
            sb::setFullScreen(true);
            cout << "fullscreen=true" << endl;
        } else if (args[i] == "--scale") {
            scale = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "scale=(" << scale.x << "," << scale.y << ")" << endl;
        } else if (args[i] == "--move") {
            offset = sf::Vector2f(stof(args[i + 1]), stof(args[i + 2]));
            cout << "offset=(" << offset.x << "," << offset.y << ")" << endl;
        }
    }

    // initialize
    cout << "Starting..." << endl;
    sb::initWindow();
    sb::RenderShakebot *render = bot.getRender();
    render->scale(scale);
    render->move(offset);

    sb::startSpeech();

    sb::Command::loadLanguage();

    audio = sb::initAudio();
    if (audio == NULL) {
        cerr << "Could not initialize audio client" << endl;
        return 1;
    }

    cout << "[running]" << endl;

    sf::RenderWindow *window = sb::getWindow();
    while (running) {
        sb::pollInput();
        bot.update();
        render->draw(window);
        sb::display();
    }

    // shutdown
    cout << "Shutting down..." << endl;
    sb::stopSpeech();
    cout << "Goodbye." << endl;

    return 0;
}

namespace sb {

    Shakebot* getBot() {
        return &bot;
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

    bool getWikiInfo(string subject, json &result) {
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

        cout << "debug1" << endl;

        RestResponse *response = client.get(url, "application/json; charset=utf-8");
        if (response == NULL) {
            cerr << "Could not perform GET request" << endl;
            return false;
        }

        cout << "debug2" << endl;

        result = response->asJson();

        cout << "debug3" << endl;

        return true;
    }

}
