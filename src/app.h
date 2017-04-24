#ifndef SHAKESPEARE_APP_H
#define SHAKESPEARE_APP_H

#include "graphics/RenderShakebot.h"
#include "audio/audio.h"

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

    /**
     * Callback for when audio recordings have finished.
     *
     * @param data Recorded data
     */
    void onRecordFinished(const AudioData *data);

}

#endif //SHAKESPEARE_APP_H
