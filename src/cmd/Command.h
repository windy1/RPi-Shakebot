#ifndef SHAKESPEARE_VOICECOMMAND_H
#define SHAKESPEARE_VOICECOMMAND_H

#include "../sb.h"
#include <vector>
#include <memory>

namespace sb {

    class Command;

    typedef shared_ptr<Command> cmd_ptr;

    /**
     * Represents a voice command.
     */
    class Command {
    public:

        static const string GREETING;
        static const string WIKI;

        /**
         * Returns the type of command this is.
         *
         * @return Command type
         */
        virtual string getType() const = 0;

        /**
         * Executes and returns the result of this command.
         *
         * @return Result of command
         */
        virtual string execute() = 0;

        /**
         * Returns a new command given a command string.
         *
         * @param raw Raw command string
         * @return New command
         */
        static cmd_ptr parse(string raw);

        /**
         * Returns the responses to greetings.
         *
         * @return Response greetings
         */
        static const vector<string>& getGreetingResponses();

        /**
         * Loads all language files.
         *
         * @return True if successful
         */
        static bool loadLanguage();

    };

}

#endif //SHAKESPEARE_VOICECOMMAND_H
