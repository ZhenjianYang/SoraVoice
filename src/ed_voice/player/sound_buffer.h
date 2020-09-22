#ifndef __PLAYER_SOUND_BUFFER_H__
#define __PLAYER_SOUND_BUFFER_H__

#include <memory>
#include <vector>

#include "player/player_base.h"
#include "utils/events.h"

namespace player {
class SoundBuffer {
public:
    class Buffer {
    public:
        virtual BuffByte* Get() const = 0;
        virtual std::size_t Samples() const = 0;
        virtual ~Buffer() = default;
    };

    virtual std::size_t GetBuffersNum() const = 0;
    virtual std::size_t GetSamplesSingleBuffer() const = 0;
    virtual std::size_t GetSamplesAllBuffers() const = 0;
    virtual std::unique_ptr<Buffer> GetBufferForWrite(std::size_t buffer_index) const = 0;
    virtual std::size_t GetPosition() const = 0;

    virtual bool Play() const = 0;
    virtual bool Stop() const = 0;
    virtual bool SetVolume(int volume) const = 0;
    virtual bool AddNewBufferEvent(utils::RawEvent event) const = 0;
    virtual bool AddPositionEvent(utils::RawEvent event, std::size_t pos) const = 0;
    virtual bool AddPositionsEvent(utils::RawEvent event,
                                   const std::vector<std::size_t>& pos) const = 0;

    virtual ~SoundBuffer() = default;
public:
    static std::unique_ptr<SoundBuffer> CreateSoundBuffer(void* pDS, const WaveFormat& wave_format);
};  // SoundBuffer
}  // namespace player

#endif  // __PLAYER_SOUND_BUFFER_H__
