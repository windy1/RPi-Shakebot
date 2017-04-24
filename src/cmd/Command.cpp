#include <fstream>
#include <iostream>
#include "Command.h"
#include "GreetingCommand.h"

namespace sb {

    const string Command::GREETING = "greeting";

    static vector<string> greetingQualifiers;
    static vector<string> greetingResponses;

    static bool readLines(string fileName, vector<string> &buffer);

    cmd_ptr Command::parse(string raw) {
        for (string qualifier : greetingQualifiers) {
            if (raw.substr(0, qualifier.size()) == qualifier) {
                // command is a greeting
                return make_shared<GreetingCommand>();
            }
        }
        return NULL;
    }

    const vector<string>& Command::getGreetingResponses() {
        return greetingResponses;
    }

    bool Command::loadLanguage() {
        cout << "Loading commands" << endl;
        bool success = true;
        success &= readLines("greetings.txt", greetingResponses);
        success &= readLines("greetings_q.txt", greetingQualifiers);
        return success;
    }

    static bool readLines(string fileName, vector<string> &buffer) {
        cout << "- " << fileName << endl;
        ifstream file(fileName);
        if (!file) {
            cerr << "Failed to open " << fileName << endl;
            return false;
        }
        string ln;
        while (getline(file, ln) && !ln.empty()) {
            buffer.push_back(ln);
        }
        file.close();
        return true;
    }

}
