#ifndef SHAKESPEARE_SEARCHCOMMAND_H
#define SHAKESPEARE_SEARCHCOMMAND_H

#include "Command.h"

namespace sb {

    class WikiCommand : public Command {

        string subject;

    public:

        static const int MAX_RESULT_SIZE = 200;

        WikiCommand(string subject);

        virtual string getType() const override;

        virtual string execute() override;

    };

}

#endif //SHAKESPEARE_SEARCHCOMMAND_H
