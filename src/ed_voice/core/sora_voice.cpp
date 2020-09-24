#include "sora_voice.h"

#include <memory>

#include "global/global.h"
#include "player/player.h"
#include "utils/create_dsound.h"
#include "utils/log.h"

namespace {
using player::Player;

class SoraVoiceImpl : public SoraVoice {
public:
    SoraVoiceImpl(const std::string& title, const std::string& built_date,
                  std::vector<std::unique_ptr<char[]>>&& movable_strs);
    void Play(byte* b) override;

private:
    std::unique_ptr<Player> player_;
    const std::string title_;
    const std::string built_date_;
    const std::vector<std::unique_ptr<char[]>> movable_strs_;

public:
    bool IsValid() const {
        return is_valid_;
    }

private:
    bool is_valid_ = false;

    SoraVoiceImpl(const SoraVoiceImpl&) = delete;
    SoraVoiceImpl operator=(const SoraVoiceImpl&) = delete;
    SoraVoiceImpl(SoraVoiceImpl&&) = delete;
    SoraVoiceImpl operator=(SoraVoiceImpl&&) = delete;
};

SoraVoiceImpl::SoraVoiceImpl(const std::string& title, const std::string& built_date,
                             std::vector<std::unique_ptr<char[]>>&& movable_strs)
    : title_{ title }, built_date_{ built_date }, movable_strs_{ std::move(movable_strs) }{
    if (!global.addrs.pHwnd || !*global.addrs.pHwnd) {
        LOG("No hwnd.");
        return;
    }
    LOG("pHwnd = 0x%08X, Hwnd = 0x%08X",
        (unsigned)global.addrs.pHwnd, (unsigned)*global.addrs.pHwnd);
    LOG("DirectSound8 Creating...");
    void* pDS8 = utils::CreateDSound8(*global.addrs.pHwnd);
    if (!pDS8) {
        LOG("DirectSound8 Create Failed!");
        return;
    }
    LOG("DirectSound8 Create Succeeded.");
    LOG("Player Creating...");
    player_ = Player::GetPlayer(pDS8);
    if (!player_) {
        LOG("Player Create Failed!");
        return;
    }
    LOG("Player Create Succeeded.");

    is_valid_ = true;
}
void SoraVoiceImpl::Play(byte*) {
}
// SoraVoiceImpl
}  // namespace

std::unique_ptr<SoraVoice> SoraVoice::GetSoraVoice(
        const std::string& title, const std::string& built_date,
        std::vector<std::unique_ptr<char[]>>&& movable_strs) {
    auto sora_voice = std::make_unique<SoraVoiceImpl>(title, built_date, std::move(movable_strs));
    return sora_voice->IsValid() ? std::move(sora_voice) : nullptr;
}
