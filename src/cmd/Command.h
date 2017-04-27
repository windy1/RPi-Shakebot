#ifndef SHAKESPEARE_VOICECOMMAND_H
#define SHAKESPEARE_VOICECOMMAND_H

#include "../sb.h"
#include <vector>
#include <memory>

namespace sb {

    class Command;

    typedef shared_ptr<Command> cmd_ptr;

    class Command {
    public:

        static const string GREETING;
        static const string WIKI;

        virtual string getType() const = 0;

        virtual string execute() = 0;

        static cmd_ptr parse(string raw);

        static const vector<string>& getGreetingResponses();

        static bool loadLanguage();

    };

}

#endif //SHAKESPEARE_VOICECOMMAND_H
