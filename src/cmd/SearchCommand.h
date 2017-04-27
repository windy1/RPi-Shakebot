#ifndef SHAKESPEARE_SEARCHCOMMAND_H
#define SHAKESPEARE_SEARCHCOMMAND_H

#include "Command.h"

namespace sb {

    class SearchCommand : public Command {

        string subject;

    public:

        SearchCommand(string subject);

        virtual string getType() const override;

        virtual string execute() override;

    };

}

#endif //SHAKESPEARE_SEARCHCOMMAND_H
