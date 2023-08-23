#include "player/decoder.h"
#include "player/decoders.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include "utils/log.h"

#define CLEAN_AND_SET_NULL(var, clean_fun) { if (var) { clean_fun(&var);} var = nullptr;}

namespace {
using player::Decoder;
using player::BuffByte;

constexpr int SAMPLES_IN_BUFF = 1024 * 4;

class Ffmpeg : public Decoder {
public:
    bool Open(const char* file_name) override {
        LOG("FFmpeg open: %s ...", file_name);
        if (int ret = avformat_open_input(&fmt_ctx_, file_name, nullptr, nullptr); ret != 0) {
            LOG("avformat_open_input failed");
            return false;
        }
        avformat_find_stream_info(fmt_ctx_, NULL);

        AVStream* stream = nullptr;
        for (int i = 0; i < (int)fmt_ctx_->nb_streams; ++i) {
            if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                stream = fmt_ctx_->streams[i];
                break;
            }
        }
        if (!stream) {
            LOG("No stream.");
            return false;
        }
        stream_index_ = stream->index;

        const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
        if (!codec) {
            LOG("avcodec_find_decoder found no codec.");
            return false;
        }

        codec_ctx_ = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(codec_ctx_, stream->codecpar) < 0) {
            LOG("avcodec_parameters_to_context failed");
            return false;
        }

        if (avcodec_open2(codec_ctx_, codec, nullptr) != 0) {
            LOG("avcodec_open2 failed");
            return false;
        }

        wave_format_.format_tag = 1;
        wave_format_.channels = codec_ctx_->ch_layout.nb_channels;
        wave_format_.samples_per_sec = codec_ctx_->sample_rate;
        wave_format_.bits_per_sample = 16;
        wave_format_.block_align = static_cast<uint16_t>(wave_format_.channels * wave_format_.bits_per_sample / 8);
        wave_format_.avg_bytes_per_sec = wave_format_.samples_per_sec * wave_format_.block_align;

        if (swr_alloc_set_opts2(&swr_ctx_, &codec_ctx_->ch_layout, AV_SAMPLE_FMT_S16, wave_format_.samples_per_sec,
            &codec_ctx_->ch_layout, codec_ctx_->sample_fmt, codec_ctx_->sample_rate, 0, NULL)) {
            printf("swr_alloc_set_opts2 failed\n");
            return false;
        }
        swr_init(swr_ctx_);

        frame_ = av_frame_alloc();
        buf_ = std::make_unique<uint8_t[]>(wave_format_.block_align * SAMPLES_IN_BUFF);
        read_frame_ = true;

        av_dump_format(fmt_ctx_, stream_index_, NULL, 0);
        samples_total_ = fmt_ctx_->duration * wave_format_.samples_per_sec / AV_TIME_BASE;
        return true;
    }

    int Read(BuffByte* buff, int samples) override {
        const int bytes_per_sample = wave_format_.channels * wave_format_.bits_per_sample / 8;
        memset(buff, 0, bytes_per_sample * samples);

        int read = 0;
        if (buf_start_ < buf_end_) {
            int samples_in_buf = (buf_end_ - buf_start_) / bytes_per_sample;
            if (samples_in_buf >= samples) {
                memcpy(buff, buf_.get() + buf_start_, samples * bytes_per_sample);
                read += samples;
                buf_start_ += samples * bytes_per_sample;
            }
            else {
                memcpy(buff, buf_.get() + buf_start_, buf_end_ - buf_start_);
                read += samples_in_buf;
                buf_start_ = buf_end_ = 0;
            }
        }

        while (read < samples) {
            if (read_frame_) {
                if (av_read_frame(fmt_ctx_, &packet_) != 0) {
                    break;
                }
                if (packet_.stream_index == stream_index_) {
                    if (avcodec_send_packet(codec_ctx_, &packet_) != 0) {
                        LOG("avcodec_send_packet failed");
                        break;
                    }
                    read_frame_ = false;
                }
            }
            if (!read_frame_) {
                uint8_t* out_buf = buf_.get();
                int aval = SAMPLES_IN_BUFF * bytes_per_sample;
                while (read < samples) {
                    if (avcodec_receive_frame(codec_ctx_, frame_) != 0) {
                        read_frame_ = true;
                        av_packet_unref(&packet_);
                        break;
                    }
                    int new_samples = swr_convert(swr_ctx_, &out_buf, aval,
                        (const uint8_t**)frame_->extended_data, frame_->nb_samples);
                    if (new_samples < 0) {
                        LOG("swr_convert failed");
                        break;
                    }
                    if (new_samples > samples - read) {
                        memcpy(buff + read * bytes_per_sample, buf_.get(), (samples - read) * bytes_per_sample);
                        buf_start_ = (samples - read) * bytes_per_sample;
                        buf_end_ = new_samples * bytes_per_sample;
                        read += samples - read;
                    }
                    else {
                        memcpy(buff + read * bytes_per_sample, buf_.get(), new_samples * bytes_per_sample);
                        read += new_samples;
                    }
                }
            }
            
        }

        samples_read_ += read;
        return read;
    }

    void Close() override {
        CLEAN_AND_SET_NULL(frame_, av_frame_free);
        CLEAN_AND_SET_NULL(codec_ctx_, avcodec_free_context);
        CLEAN_AND_SET_NULL(fmt_ctx_, avformat_close_input);
        CLEAN_AND_SET_NULL(swr_ctx_, swr_free);
    }

    Ffmpeg() = default;
    ~Ffmpeg() override {
        this->Close();
    }
private:
    AVFormatContext* fmt_ctx_{ nullptr };
    AVCodecContext* codec_ctx_{ nullptr };
    SwrContext* swr_ctx_{ nullptr };
    AVFrame* frame_{ nullptr };
    AVPacket packet_{};
    bool read_frame_{};
    int stream_index_{ 0 };

    std::unique_ptr<uint8_t[]> buf_{ nullptr };
    int buf_start_{ 0 }, buf_end_{ 0 };

    Ffmpeg(const Ffmpeg&) = delete;
    Ffmpeg& operator=(const Ffmpeg&) = delete;
    Ffmpeg(Ffmpeg&&) = delete;
    Ffmpeg& operator=(Ffmpeg&&) = delete;
};  // Ffmpeg

static bool Init() {
    return true;
}

static std::unique_ptr<player::Decoder> Get() {
    return std::make_unique<Ffmpeg>();
}

}// namesapce

namespace player::impl {
extern const DecoderId DecoderFfmpeg {
    "ffmpeg",
    &Init,
    &Get
};
}  // namespace player::impl
