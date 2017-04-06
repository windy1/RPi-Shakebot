#include "flac.h"
#include <iostream>
#include <cinttypes>
#include <fstream>

#define READ_SIZE 1024

using namespace std;

namespace sb {

    static uint32_t totalSamples = 0;
    static FLAC__int32 pcm[READ_SIZE * NUM_CHANNELS];

    static void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written,
                                  FLAC__uint64 samples_written, unsigned frames_written,
                                  unsigned total_frames_estimate, void *client_data);

    static FLAC__StreamEncoderWriteStatus write_callback(const FLAC__StreamEncoder *encoder,
                                                         const FLAC__byte buffer[],
                                                         size_t bytes, unsigned samples,
                                                         unsigned current_frame, void *client_data);


    bool encodeFlac(const AudioData &data, const char *fileName) {
        bool success = true;
        FLAC__StreamEncoder *encoder = 0;
        FLAC__StreamEncoderInitStatus initStatus;
        FLAC__StreamMetadata *metadata[2];
        FLAC__StreamMetadata_VorbisComment_Entry entry;
        uint32_t sampleRate = SAMPLE_RATE;
        uint32_t channels = NUM_CHANNELS;
        uint32_t bitsPerSample = 16;
        totalSamples = (uint32_t) data.maxFrameIndex * NUM_CHANNELS;

        size_t samplesLeft = (size_t) totalSamples;
        //FILE *file = fopen(fileName, "w+b");
        //ifstream file;
        cout << "samplesLeft = " << samplesLeft << endl;

        // allocate encoder
        if ((encoder = FLAC__stream_encoder_new()) == NULL) {
            cout << "Error: Could not allocate encoder." << endl;
            success = false;
            goto done;
        }

        success &= FLAC__stream_encoder_set_verify(encoder, true);
        cout << success << endl;
        success &= FLAC__stream_encoder_set_compression_level(encoder, 5);
        cout << success << endl;
        success &= FLAC__stream_encoder_set_channels(encoder, channels);
        cout << success << endl;
        success &= FLAC__stream_encoder_set_bits_per_sample(encoder, bitsPerSample);
        cout << success << endl;
        success &= FLAC__stream_encoder_set_sample_rate(encoder, sampleRate);
        cout << success << endl;
        success &= FLAC__stream_encoder_set_total_samples_estimate(encoder, totalSamples);
        cout << success << endl;

        if (!success) {
            cout << "Error: Could not initialize encoder." << endl;
            goto done;
        }

        if ((metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL
            || (metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING)) == NULL
            || !FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", "Some Artist")
            || !FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, false)
            || !FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR", "1984")
            || !FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, false)) {
            cout << "Error: Could allocate metadata." << endl;
            success = false;
            goto done;
        }
        metadata[0]->length = 1234;
        if (!FLAC__stream_encoder_set_metadata(encoder, metadata, 2)) {
            cout << "Error: Could not set metadata." << endl;
            success = false;
            goto done;
        }

//        if (file != NULL) {
//            cout << "file opened" << endl;
//        } else {
//            cout << "file not opened" << endl;
//            success = false;
//            goto done;
//        }

        // initialize encoder
        initStatus = FLAC__stream_encoder_init_file(encoder, fileName, progress_callback, NULL);
        //initStatus = FLAC__stream_encoder_init_FILE(encoder, file, progress_callback, NULL);
        if(initStatus != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
            cout << "Error: Invalid init status." << endl;
            cout << "Message: " << FLAC__StreamEncoderInitStatusString[initStatus] << endl;
            cout << FLAC__stream_encoder_get_resolved_state_string(encoder) << endl;
            success = false;
            goto done;
        }

        while (samplesLeft) {
            size_t toRead = samplesLeft > READ_SIZE ? (size_t) READ_SIZE : (size_t) samplesLeft;
            for (size_t i = 0; i < toRead * channels; i++) {
                pcm[i] = (FLAC__int32)(((FLAC__int16)(FLAC__int8) data.recordedSamples[2 * i + 1] << 8)
                                       | (FLAC__int16) data.recordedSamples[2 * i]);
            }
            if (!FLAC__stream_encoder_process_interleaved(encoder, pcm, toRead)) {
                cout << "Error: Could not process samples." << endl;
                success = false;
                goto done;
            }
            samplesLeft -= toRead;
        }

        if (!FLAC__stream_encoder_finish(encoder)) {
            cout << "Error: Could not finish encoding.";
            success = false;
            goto done;
        }

        done:
        if (!success) {
            cout << "State: " << FLAC__StreamEncoderStateString[FLAC__stream_encoder_get_state(encoder)] << endl;
            cout << "[failed]" << endl;
        } else {
            cout << "[success]" << endl;
        }
        FLAC__metadata_object_delete(metadata[0]);
        FLAC__metadata_object_delete(metadata[1]);
        FLAC__stream_encoder_delete(encoder);
        return success;
    }

    void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written,
                           FLAC__uint64 samples_written, unsigned frames_written,
                           unsigned total_frames_estimate, void *client_data) {
        (void) encoder;
        (void) client_data;
        fprintf(stderr, "wrote %" PRIu64 " bytes, %" PRIu64 "/%u samples, %u/%u frames\n",
                bytes_written, samples_written, totalSamples, frames_written, total_frames_estimate);
    }

    FLAC__StreamEncoderWriteStatus write_callback(const FLAC__StreamEncoder *encoder,
                                                  const FLAC__byte buffer[],
                                                  size_t bytes, unsigned samples,
                                                  unsigned current_frame, void *client_data) {

    }

}
