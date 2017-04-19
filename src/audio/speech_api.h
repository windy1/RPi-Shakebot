#ifndef SHAKESPEARE_VOICE_API_H
#define SHAKESPEARE_VOICE_API_H

#include "audio.h"
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

namespace sb {

    /**
     * Performs a synchronous request to Google Speech API with the specified
     * AudioData to be translated to text.
     *
     * @param data Data to send to Google
     * @return JSON response from Google
     */
    bool speech2text(const AudioData *data, json &result);

}

#endif //SHAKESPEARE_VOICE_API_H
