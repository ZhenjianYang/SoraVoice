#include "player/decoder.h"

#include <fstream>

namespace player::impl {

class Wav : public Decoder {
public:
    bool Open(const char* file_name) override;
    std::size_t Read(BuffByte* buff, std::size_t samples_count) override;
    void Close() override;

    Wav() = default;
    ~Wav() override {
        this->Close();
    }
private:
    std::ifstream ifs_;

    Wav(const Wav&) = delete;
    Wav& operator=(const Wav&) = delete;
    Wav(Wav&&) = delete;
    Wav& operator=(Wav&&) = delete;

public:
    static std::unique_ptr<Decoder> Get();
    static bool Init();
};  // Wav
}  // namesapce player::impl
