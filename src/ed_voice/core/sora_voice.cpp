#include "sora_voice.h"

#include <memory>
#include <mutex>
#include <unordered_set>

#include "core/load_config.h"
#include "core/rnd_voice.h"
#include "core/voice_id_mapping.h"
#include "global/global.h"
#include "player/player.h"
#include "utils/build_date.h"
#include "utils/create_dsound.h"
#include "utils/log.h"
#include "utils/win_title.h"

namespace {
enum {
    kOK = 0,
    kError = -1
};
constexpr int kNumMapping = sizeof(kVoiceIdMapping) / sizeof(*kVoiceIdMapping);
constexpr int kBgmVoiceIdLen = 6;
static_assert(kBgmVoiceIdLen > kMaxVoiceIdLenNeedMapping, "kBgmVoiceIdLen <= kMaxVoiceIdLenNeedMapping !!!");
constexpr int kBgmVoiceFilenameLen = 4;
constexpr int kOriVoiceIdLen = 4;
constexpr char kVoicePrefixOri[] = "data/se/ed7v";
constexpr char kVoicePrefixED6[] = "voice/ogg/ch";
constexpr char kVoicePrefixBGM[] = "voice/ogg/ed";
constexpr char kVoicePrefixZA[] = "voice/ogg/v";
constexpr char kAttrOgg[] = ".ogg";
constexpr char kAttrWav[] = ".wav";
constexpr char kConfigFilename[] = "voice/ed_voice.ini";
constexpr std::size_t kAoRndVoiceDelayMs = 1000;

constexpr const char* kInfo[] = { " - SoraVoice (Lite) ", kBuildDate };
}

namespace {
using player::Player;

class SoraVoiceImpl : public SoraVoice {
public:
    SoraVoiceImpl(const std::string& title, const std::string& built_date,
                  std::vector<std::unique_ptr<char[]>>&& movable_strs);
    int Play(byte* b) override;
    int Stop() override;

private:
    const std::string title_;
    const std::string built_date_;
    const std::vector<std::unique_ptr<char[]>> movable_strs_;
    Signals* const sigs_;
    Config* const config_;
    const Info* const info_;
    std::unique_ptr<Player> player_;

    std::unordered_set<Player::PlayId> playing_;
    std::mutex mtx_playing_;

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
    : title_{ title }, built_date_{ built_date }, movable_strs_{ std::move(movable_strs) },
      sigs_{ &global.sigs }, config_{ &global.config }, info_{ &global.info } {
    LOG("Game = %d", info_->game);

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

    core::LoadConfig(config_, kConfigFilename);
    if (config_->volume > player::kVolumeMax) {
        config_->volume = player::kVolumeMax;
    }
    if (config_->volume < 0) {
        config_->volume = 0;
    }
    core::SaveConfig(config_, kConfigFilename, info_);
    LOG("Config:\n"
        "    Volume = %d\n"
        "    DisableTextSe = %d\n"
        "    DisableDialogSe = %d\n"
        "    DisableAoOriVoice = %d",
        config_->volume, config_->disable_text_se, config_->disable_ao_ori_voice,
        config_->disable_ao_ori_voice);
    player_->SetVolume(config_->volume);

    memset(sigs_, 0, sizeof(*sigs_));
    is_valid_ = true;

    if (config_->show_info) {
        std::string win_title = utils::GetWinTitle(*global.addrs.pHwnd);
        LOG("Window title: %s", title.c_str());
        for (auto info : kInfo) {
            win_title.append(info);
        }
        if (utils::SetWinTitle(*global.addrs.pHwnd, win_title)) {
            LOG("New window title: %s", win_title.c_str());
        }
    }

    if (info_->game == GameAo) {
        LOG("Play random voice for ao...");
        player_->Play(core::GetRandomVoice(), nullptr, kAoRndVoiceDelayMs);
    }
}
int SoraVoiceImpl::Play(byte* b) {
    if (*b != '#') {
        return kError;
    }
    b++;

    int num_vid = 0;
    byte* e = b;
    for (int i = 0; i < kMaxVoiceIdLen; i++, e++) {
        if (*e < '0' || *e > '9') {
            break;
        }
        num_vid *= 10;
        num_vid += *e - '0';
    }
    if (*e == 'V' && e - b == kOriVoiceIdLen && info_->game == GameAo) {
        if (config_->disable_ao_ori_voice) {
            LOG("Disable ori voice: %s", std::string(b, e).c_str());
            for (byte* t = b; t < e; t++) {
                *t = '0';
            }
        } else {
            LOG("Ori voice: %s, stop current playing", std::string(b, e).c_str());
            player_->StopAll();
            return kError;
        }
    }

    if (*e != 'v' || b == e) {
        return kError;
    }

    std::string vid(b, e);
    LOG("input Voice ID is '%s'", vid.c_str());
    LOG("The max length of voice id need mapping is %d", kMaxVoiceIdLenNeedMapping);
    if (info_->game != GameAo && info_->game != GameZero
        && e - b <= kMaxVoiceIdLenNeedMapping) {
        num_vid += kVoiceIdAdjustAdder[e - b];
        LOG("Adjusted Voice ID is %d", num_vid);
        LOG("Number of mapping is %d", kNumMapping);
        if (num_vid >= kNumMapping) {
            LOG("Adjusted Voice ID is out of the range of Mapping");
            return kError;
        }

        vid = kVoiceIdMapping[num_vid];
        if (vid.empty()) {
            LOG("Mapping Voice ID is empty");
            return kError;
        }
    }

    const char* prefix =
            (info_->game == GameAo || info_->game == GameZero) ? kVoicePrefixZA
                              : vid.length() == kBgmVoiceIdLen ? kVoicePrefixBGM
                                                               : kVoicePrefixED6;

    if (info_->game != GameAo && info_->game != GameZero
        && vid.length() == kBgmVoiceIdLen) {
        vid = vid.substr(kBgmVoiceIdLen - kBgmVoiceFilenameLen);
    }

    std::string ogg_file_name = prefix + vid + kAttrOgg;

    player_->StopAll();
    {
        std::scoped_lock lock(mtx_playing_);
        auto play_id = player_->Play(ogg_file_name,
                                     [this](Player::PlayId play_id, Player::StopType stop_type) {
                LOG("PlayID: %d, stopped, type: %d", play_id, stop_type);
                std::scoped_lock lock(mtx_playing_);
                playing_.erase(play_id);
                if (playing_.empty()) {
                    sigs_->no_textse = 0;
                    if (stop_type == Player::StopType::Error) {
                        sigs_->no_dlgse = 0;
                    }
                }
            });
        playing_.insert(play_id);
        sigs_->no_dlgse = 1;
        sigs_->no_textse = 1;
    }
    return kOK;
}
int SoraVoiceImpl::Stop() {
    return player_->StopAll();
}
// SoraVoiceImpl
}  // namespace

std::unique_ptr<SoraVoice> SoraVoice::GetSoraVoice(
        const std::string& title, const std::string& built_date,
        std::vector<std::unique_ptr<char[]>>&& movable_strs) {
    auto sora_voice = std::make_unique<SoraVoiceImpl>(title, built_date, std::move(movable_strs));
    return sora_voice->IsValid() ? std::move(sora_voice) : nullptr;
}
