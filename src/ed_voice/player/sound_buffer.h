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
        virtual int Samples() const = 0;
        virtual ~Buffer() = default;
    };

    virtual int GetBuffersNum() const = 0;
    virtual int GetSamplesSingleBuffer() const = 0;
    virtual int GetSamplesAllBuffers() const = 0;
    virtual std::unique_ptr<Buffer> GetBufferForWrite(int buffer_index) const = 0;
    virtual int GetPosition() const = 0;

    virtual bool Play() const = 0;
    virtual bool Stop() const = 0;
    virtual bool SetVolume(int volume) const = 0;
    virtual bool AddPositionsEvent(utils::RawEvent event,
                                   const std::vector<int>& pos) const = 0;

    virtual ~SoundBuffer() = default;
public:
    static std::unique_ptr<SoundBuffer> CreateSoundBuffer(void* pDS, const WaveFormat& wave_format);
};  // SoundBuffer
}  // namespace player

#endif  // __PLAYER_SOUND_BUFFER_H__
