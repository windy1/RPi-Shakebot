#ifndef SHAKESPEARE_FLAC_H
#define SHAKESPEARE_FLAC_H

#include "FLAC++/metadata.h"
#include "FLAC++/encoder.h"

namespace sb {

    class FlacEncoder : public FLAC::Encoder::File {
    public:

        FlacEncoder();

    protected:

        virtual void progress_callback(FLAC__uint64 bytes_written, FLAC__uint64 samples_written,
                                       uint32_t frames_written, uint32_t total_frames_estimate) override;

    };

}

#endif //SHAKESPEARE_FLAC_H
