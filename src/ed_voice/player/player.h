#ifndef __PLAYER_PLAYER_H__
#define __PLAYER_PLAYER_H__

#include <functional>
#include <memory>
#include <string_view>
#include <vector>
#include <string>

#include "player/player_base.h"

namespace player {
class Player {
public:
    using PlayId = unsigned;
    enum class StopType {
        PlayEnd,
        ForceStop,
        Error
    };
    using StopCallback = std::function<void(PlayId, StopType)>;

    static constexpr PlayId kInvalidPlayId = 0;

    virtual bool IsPlaying(PlayId play_id) const = 0;
    virtual std::vector<PlayId> CurrentPlaying() const = 0;

    virtual void SetVolume(int volume = kVolumeMax) = 0;
    virtual int GetVolume() const = 0;

    virtual PlayId Play(const std::vector<std::string>& filenames, StopCallback callback = nullptr, int delay_ms = 0) = 0;
    virtual PlayId Play(std::string_view filename, StopCallback callback = nullptr, int delay_ms = 0) = 0;
    virtual bool StopAll() = 0;

    virtual ~Player() = default;

    static std::unique_ptr<Player> GetPlayer(void* pDS8);
};  // Player
}  //player

#endif  // __PLAYER_PLAYER_H__
