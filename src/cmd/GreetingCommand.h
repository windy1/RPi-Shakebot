#ifndef SHAKESPEARE_GREETINGCOMMAND_H
#define SHAKESPEARE_GREETINGCOMMAND_H

#include "Command.h"

namespace sb {

    class GreetingCommand : public Command {
    public:

        virtual string getType() const override;

        virtual string execute() override;

    };

}

#endif //SHAKESPEARE_GREETINGCOMMAND_H
