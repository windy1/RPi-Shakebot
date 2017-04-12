#ifndef SHAKESPEARE_APP_H
#define SHAKESPEARE_APP_H

#include "graphics/RenderShakebot.h"

namespace sb {

    /**
     * Returns the Shakebot instance.
     *
     * @return Shakebot instance
     */
    sb::Shakebot* getBot();

    /**
     * Returns true if the application is running.
     *
     * @return True if running
     */
    bool isRunning();

    /**
     * Sets whether the application is running.
     *
     * @param running False if should stop
     */
    void setRunning(bool running);

}

#endif //SHAKESPEARE_APP_H
