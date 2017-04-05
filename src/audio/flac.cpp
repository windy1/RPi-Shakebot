#include "flac.h"

namespace sb {

    FlacEncoder::FlacEncoder() : FLAC::Encoder::File() {
    }

    void FlacEncoder::progress_callback(FLAC__uint64 bytes_written, FLAC__uint64 samples_written,
                                        uint32_t frames_written, uint32_t total_frames_estimate) {

    }

}
