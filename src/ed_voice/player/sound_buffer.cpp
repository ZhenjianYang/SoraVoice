#include "sound_buffer.h"

#include <dsound.h>
#include <cmath>

namespace {
using player::BuffByte;
using player::SoundBuffer;
using player::WaveFormat;
using player::kVolumeMax;

constexpr std::size_t kBufferNum = 2;
constexpr std::size_t kSamplesSingleBuffer = 24000;

inline static int GetDSVolume(int volume) {
    return volume <= 0 ? DSBVOLUME_MIN
        : static_cast<int>(2000 * std::log10(double(volume) / kVolumeMax));
}

class BufferImpl : public SoundBuffer::Buffer {
public:
    bool IsValid() const {
        return is_valid_;
    }
    BuffByte* Get() const override {
        return reinterpret_cast<BuffByte*>(pap1_);
    }
    std::size_t Samples() const override {
        return samples_;
    }

    BufferImpl(IDirectSoundBuffer8* pds_buffer,
               std::size_t start, std::size_t samples, std::size_t sample_size)
        : pDS_buffer8_{ pds_buffer }, start_{ start }, samples_{ samples },
          sample_size_{ sample_size } {
        auto lock = pDS_buffer8_->Lock(start_ * sample_size_, samples_ * sample_size_,
                                       &pap1_, &ab1_, &pap2_, &ab2_, NULL);
        if (lock == DS_OK && pap2_ == NULL && samples_ * sample_size == ab1_) {
            is_valid_ = true;
        } else {
            is_valid_ = false;
        }
    }
    ~BufferImpl() override {
        if (pap1_) {
            pDS_buffer8_->Unlock(pap1_, ab1_, pap2_, ab2_);
        }
    }

private:
    IDirectSoundBuffer8* const pDS_buffer8_;
    const std::size_t start_;
    const std::size_t samples_;
    const std::size_t sample_size_;

    bool is_valid_ = false;

    LPVOID pap1_ = NULL;
    DWORD ab1_ = 0;
    LPVOID pap2_ = NULL;
    DWORD ab2_ = 0;

    BufferImpl(const BufferImpl&) = delete;
    BufferImpl& operator=(const BufferImpl&) = delete;
};  // BufferImpl

class SoundBufferImpl : public SoundBuffer {
public:
    bool IsValid() const {
        return is_valid_;
    }

    std::size_t GetBuffersNum() const override {
        return num_buffer_;
    }
    std::size_t GetSamplesSingleBuffer() const override {
        return samples_single_buffer_;
    }
    std::size_t GetSamplesAllBuffers() const override {
        return GetBuffersNum() * GetSamplesSingleBuffer();
    }
    std::unique_ptr<Buffer> GetBufferForWrite(std::size_t buffer_index) const override {
        if (buffer_index >= GetBuffersNum()) {
            return nullptr;
        }
        auto buffer = std::make_unique<BufferImpl>(pDS_buffer8_, buffer_index * GetSamplesSingleBuffer(),
                                                   GetSamplesSingleBuffer(), wave_format_.block_align);
        return buffer->IsValid() ? std::move(buffer) : nullptr;
    }
    std::size_t GetPosition() const override {
        DWORD pos;
        pDS_buffer8_->GetCurrentPosition(&pos, NULL);
        return pos / wave_format_.block_align;
    }

    bool Play() const override {
        return DS_OK == pDS_buffer8_->Play(0, 0, DSBPLAY_LOOPING);
    }
    bool Stop() const override {
        return DS_OK == pDS_buffer8_->Stop();
    }
    bool SetVolume(int volume) const override {
        if (volume > kVolumeMax) {
            return false;
        }
        return DS_OK == pDS_buffer8_->SetVolume(GetDSVolume(volume));
    }
    bool AddPositionsEvent(utils::RawEvent event,
                           const std::vector<std::size_t>& pos) const override {
        LPDIRECTSOUNDNOTIFY8 pDS_notify;

        auto hr = pDS_buffer8_->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&pDS_notify);
        if (!SUCCEEDED(hr)) {
            return false;
        }
        auto notifis = std::make_unique<DSBPOSITIONNOTIFY[]>(pos.size());
        for (std::size_t i = 0; i < pos.size(); i++) {
            notifis[i].dwOffset = pos[i] * wave_format_.block_align;
            notifis[i].hEventNotify = reinterpret_cast<HANDLE>(event);
        }
        hr = pDS_notify->SetNotificationPositions(pos.size(), notifis.get());
        pDS_notify->Release();
        return SUCCEEDED(hr);
    }

    SoundBufferImpl(void* pDS8, const WaveFormat& wave_format)
        : pDS8_{ reinterpret_cast<IDirectSound8*>(pDS8) }, num_buffer_{ kBufferNum },
          samples_single_buffer_{ kSamplesSingleBuffer }, wave_format_{ wave_format }{
        WAVEFORMATEX wave_format_ex{};
        wave_format_ex.wFormatTag = WAVE_FORMAT_PCM;
        wave_format_ex.nChannels = wave_format_.channels;
        wave_format_ex.nSamplesPerSec = wave_format_.samples_per_sec;
        wave_format_ex.wBitsPerSample = wave_format_.bits_per_sample;
        wave_format_ex.nBlockAlign = wave_format_.block_align;
        wave_format_ex.nAvgBytesPerSec = wave_format_.avg_bytes_per_sec;
        wave_format_ex.cbSize = 0;

        DSBUFFERDESC ds_buffer_desc{};
        ds_buffer_desc.dwSize = sizeof(ds_buffer_desc);
        ds_buffer_desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
        ds_buffer_desc.dwBufferBytes = wave_format_ex.nBlockAlign * samples_single_buffer_ * num_buffer_;
        ds_buffer_desc.dwReserved = 0;
        ds_buffer_desc.lpwfxFormat = &wave_format_ex;
        ds_buffer_desc.guid3DAlgorithm = { };

        LPDIRECTSOUNDBUFFER pDS_buffer = NULL;
        auto hr = pDS8_->CreateSoundBuffer(&ds_buffer_desc, &pDS_buffer, NULL);
        if (SUCCEEDED(hr)) {
            hr = pDS_buffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pDS_buffer8_);
            pDS_buffer->Release();
        }
        is_valid_ = SUCCEEDED(hr);
    }

    ~SoundBufferImpl() override {
        if (pDS_buffer8_) {
            pDS_buffer8_->Release();
        }
    }

private:
    bool is_valid_ = false;

    IDirectSound8* const pDS8_;
    const std::size_t num_buffer_;
    const std::size_t samples_single_buffer_;
    const WaveFormat wave_format_;

    IDirectSoundBuffer8* pDS_buffer8_ = nullptr;

    SoundBufferImpl(const SoundBufferImpl&) = delete;
    SoundBufferImpl& operator=(const SoundBufferImpl&) = delete;
};  // SoundBuffer

}  // namespace

std::unique_ptr<SoundBuffer> player::SoundBuffer::CreateSoundBuffer(void* pDS, const WaveFormat& wave_format) {
    auto sound_buffer =  std::make_unique<SoundBufferImpl>(pDS, wave_format);
    return sound_buffer->IsValid() ? std::move(sound_buffer) : nullptr;
}
