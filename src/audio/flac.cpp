#include "flac.h"
#include <iostream>
#include <cinttypes>
#include <vector>

#define ARTIST "RPi-Shakebot"

using namespace std;

/*
 * Unused: Originally made as alternative to sending LINEAR16 to Google Voice API.
 * Reference: https://git.xiph.org/?p=flac.git;a=blob;f=examples/cpp/encode/file/main.cpp;h=7d44b873346af32a00c7928b98b6a6a022d737b5;hb=HEAD
 */

namespace sb {

    static uint32_t totalSamples = 0;

    static void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written,
                                  FLAC__uint64 samples_written, unsigned frames_written,
                                  unsigned total_frames_estimate, void *client_data);

    bool encodeFlac(const AudioData &data, const char *fileName) {
        bool success = true;
        FLAC__StreamEncoder *encoder = 0;
        FLAC__StreamEncoderInitStatus initStatus;
        FLAC__StreamMetadata *metadata[2];
        FLAC__StreamMetadata_VorbisComment_Entry entry;
        uint32_t sampleRate = SAMPLE_RATE;
        uint32_t channels = NUM_CHANNELS;
        uint32_t bitsPerSample = sizeof(Sample) * 8;
        totalSamples = (uint32_t) data.maxFrameIndex * NUM_CHANNELS;

        time_t tm = time(NULL);
        int year = localtime(&tm)->tm_year + 1900;

        cout << "sampleRate = " << sampleRate << endl;
        cout << "bitsPerSample = " << bitsPerSample << endl;
        cout << "totalSamples = " << totalSamples << endl;

        // transfer data to int32 buffer
        vector<int32_t> buffer(totalSamples);
        for (int i = 0; i < totalSamples; i++) {
            buffer[i] = (FLAC__uint32) data.recordedSamples[i];
        }

        // allocate encoder
        if ((encoder = FLAC__stream_encoder_new()) == NULL) {
            cout << "Error: Could not allocate encoder." << endl;
            success = false;
            goto done;
        }

        success &= FLAC__stream_encoder_set_verify(encoder, false);
        success &= FLAC__stream_encoder_set_compression_level(encoder, 5);
        success &= FLAC__stream_encoder_set_channels(encoder, channels);
        success &= FLAC__stream_encoder_set_bits_per_sample(encoder, bitsPerSample);
        success &= FLAC__stream_encoder_set_sample_rate(encoder, sampleRate);
        success &= FLAC__stream_encoder_set_total_samples_estimate(encoder, totalSamples / 2);

        if (!success) {
            cout << "Error: Could not initialize encoder." << endl;
            goto done;
        }

        // set some metadata
        if ((metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL
            || (metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING)) == NULL
            || !FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", ARTIST)
            || !FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, false)
            || !FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR", to_string(year).data())
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

        // initialize encoder
        initStatus = FLAC__stream_encoder_init_file(encoder, fileName, progress_callback, NULL);
        if(initStatus != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
            cout << "Error: Invalid init status." << endl;
            cout << "Message: " << FLAC__StreamEncoderInitStatusString[initStatus] << " ";
            cout << FLAC__stream_encoder_get_resolved_state_string(encoder) << endl;
            success = false;
            goto done;
        }

        // process samples
        if (!FLAC__stream_encoder_process_interleaved(encoder, &buffer[0], totalSamples / 2)) {
            cout << "Error: Could not process samples." << endl;
            success = false;
            goto done;
        }

        // finish encoding
        if (!FLAC__stream_encoder_finish(encoder)) {
            cout << "Error: Could not finish encoding.";
            success = false;
            goto done;
        }

        // cleanup
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

}
