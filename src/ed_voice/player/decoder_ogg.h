#include "player/decoder.h"

#include <vorbis/vorbisfile.h>

namespace player::impl {

class Ogg : public Decoder {
public:
    bool Open(const char* file_name) override;
    std::size_t Read(BuffByte* buff, std::size_t samples_count) override;
    void Close() override;

    Ogg() = default;
    ~Ogg() override {
        this->Close();
    }

private:
    OggVorbis_File ov_file{};

    Ogg(const Ogg&) = delete;
    Ogg& operator=(const Ogg&) = delete;
    Ogg(Ogg&&) = delete;
    Ogg& operator=(Ogg&&) = delete;

public:
    static std::unique_ptr<Decoder> Get();
    static bool Init();
};  // Ogg
}  // namesapce player::impl
