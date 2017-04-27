#include "GreetingCommand.h"

namespace sb {

    string GreetingCommand::getType() const {
        return GREETING;
    }

    string GreetingCommand::execute() {
        const vector<string> &responses = Command::getGreetingResponses();
        return responses[rand() % responses.size()];
    }

}
