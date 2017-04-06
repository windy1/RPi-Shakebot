#ifndef SHAKESPEARE_FLAC_H
#define SHAKESPEARE_FLAC_H

#include "FLAC++/metadata.h"
#include "FLAC++/encoder.h"
#include "record.h"

namespace sb {

    bool encodeFlac(const AudioData &data, const char *fileName);

}

#endif //SHAKESPEARE_FLAC_H
