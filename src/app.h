#ifndef SHAKESPEARE_APP_H
#define SHAKESPEARE_APP_H

#include "graphics/RenderShakebot.h"

namespace sb {

    sb::Shakebot* getBot();

    bool isRunning();

    void setRunning(bool running);

}

#endif //SHAKESPEARE_APP_H
