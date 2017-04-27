#ifndef SHAKESPEARE_VOICECOMMAND_H
#define SHAKESPEARE_VOICECOMMAND_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

namespace sb {

    class Command;

    typedef shared_ptr<Command> cmd_ptr;

    class Command {
    public:

        static const string GREETING;
        static const string SEARCH;

        virtual string getType() const = 0;

        virtual string execute() = 0;

        static cmd_ptr parse(string raw);

        static const vector<string>& getGreetingResponses();

        static bool loadLanguage();

    };

}

#endif //SHAKESPEARE_VOICECOMMAND_H
