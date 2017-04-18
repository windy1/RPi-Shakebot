#ifndef SHAKESPEARE_VOICE_API_H
#define SHAKESPEARE_VOICE_API_H

#include "audio.h"
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

namespace sb {

    json speech2text(const AudioData &data);

}

#endif //SHAKESPEARE_VOICE_API_H
