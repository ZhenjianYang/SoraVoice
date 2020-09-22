#include "player.h"

#include <mutex>
#include <string>
#include <unordered_map>

#include "player/decoder.h"
#include "player/sound_buffer.h"
#include "utils/events.h"
#include "utils/create_dsound.h"
#include "utils/log.h"

namespace {
using player::kVolumeMax;
using player::Decoder;
using player::Player;
using player::SoundBuffer;
using utils::Events;
constexpr char kAttrWav[] = ".wav";
constexpr char kAttrOgg[] = ".ogg";
constexpr std::size_t kEventIndexNewPlay = 0;
constexpr std::size_t kEventIndexReadOrEnd = 1;
constexpr std::size_t kEventIndexStopAll = 2;
constexpr std::size_t kEventIndexCallback = 3;
constexpr std::size_t kEventIndexSetVolumn = 4;
constexpr std::size_t kEventIndexExit = 5;
constexpr std::size_t kEventsNum = 6;
constexpr std::size_t kNotEnd = ~0U;

static std::unique_ptr<Decoder> GetDecoderByFilename(std::string_view file_name) {
    auto pos = file_name.rfind('.');
    if (pos == std::string::npos) {
        return nullptr;
    }
    std::string attr = std::string(file_name.substr(pos));
    for (char& ch : attr) {
        ch = static_cast<char>(std::tolower(ch));
    }
    if (attr == kAttrWav) {
        return Decoder::GetWav();
    } else if (attr == kAttrOgg) {
        return Decoder::GetOgg();
    } else {
        return nullptr;
    }
}

class PlayerImpl : public player::Player {
public:
    bool IsValid() const {
        return is_valid_;
    }

    bool IsPlaying(PlayId play_id) const override {
        std::scoped_lock lock(mtx_pd_playing_, mtx_pd_new_);
        return pd_playing_.find(play_id) != pd_playing_.end()
            || pd_new_.find(play_id) != pd_new_.end();
    }
    std::vector<PlayId> CurrentPlaying() const override {
        std::vector<PlayId> play_ids;
        std::scoped_lock lock(mtx_pd_playing_, mtx_pd_new_);
        for (const auto& play : pd_playing_) {
            play_ids.push_back(play.first);
        }
        for (const auto& play : pd_new_) {
            play_ids.push_back(play.first);
        }
        return play_ids;
    }

    void SetVolume(int volume = kVolumeMax) override {
        std::scoped_lock lock(mtx_volumn_);
        volume_ = volume;
        events_->Set(kEventIndexSetVolumn);
    }
    int GetVolume() const override {
        std::scoped_lock lock(mtx_volumn_);
        return volume_;
    }

    PlayId Play(std::string_view file_name, StopCallback callback = nullptr) override {
        PlayId new_id = current_playid_++;
        if (new_id == kInvalidPlayId) {
            new_id = current_playid_++;
        }
        std::unique_ptr<NewData> new_data = std::make_unique<NewData>(
            new_id, std::string(file_name), callback);
        {
            std::scoped_lock lock(mtx_pd_new_);
            pd_new_[new_data->play_id] = std::move(new_data);
        }
        events_->Set(kEventIndexNewPlay);
        return new_id;
    }
    bool StopAll() override {
        events_->Set(kEventIndexStopAll);
        return true;
    }

    PlayerImpl(void* pDS) : pDS_{ pDS }, events_{ Events::CreateEvents(kEventsNum) } {
        if (!events_) {
            return;
        }
        is_valid_ = true;
        th_worker_ = std::thread(&PlayerImpl::EventWorker, this);
    }
    ~PlayerImpl() {
        is_valid_ = false;
        if (events_) {
            events_->Set(kEventIndexExit);
        }
        if (th_worker_.joinable()) {
            th_worker_.join();
        }
    }

private:
    bool is_valid_ = false;
    PlayId current_playid_ = kInvalidPlayId + 1;

    struct NewData {
        PlayId play_id = kInvalidPlayId;
        std::string file_name;
        StopCallback callback;

        NewData(PlayId play_id, const std::string& file_name, StopCallback callback)
            : play_id{ play_id }, file_name{ file_name }, callback{ callback }{
        }
    };
    struct PlayData {
        PlayId play_id = kInvalidPlayId;
        StopCallback callback;

        std::unique_ptr<Decoder> decoder;
        std::unique_ptr<SoundBuffer> buffer;

        std::size_t buffer_index = 0;
        std::size_t end_pos = kNotEnd;
        bool stop_soon = false;
        bool playing = false;

        PlayData(PlayId play_id, StopCallback callback)
            : play_id{ play_id }, callback{ callback }{
        }
    };
    struct CallbackData {
        PlayId play_id = kInvalidPlayId;
        StopType stop_type;
        StopCallback callback;

        CallbackData(PlayId play_id, StopType stop_type, StopCallback callback)
            : play_id{ play_id }, stop_type{ stop_type }, callback{ callback }{
        }
    };
    std::unordered_map<PlayId, std::unique_ptr<NewData>> pd_new_;
    mutable std::mutex mtx_pd_new_;
    std::unordered_map<PlayId, std::unique_ptr<PlayData>> pd_playing_;
    mutable std::mutex mtx_pd_playing_;
    std::vector<std::unique_ptr<CallbackData>> pd_callback_;
    mutable std::mutex mtx_pd_callback_;

    int volume_ = kVolumeMax;
    mutable std::mutex mtx_volumn_;

    void* const pDS_;
    std::unique_ptr<Events> events_;
    std::thread th_worker_;

    void EventWorker() {
        while (is_valid_) {
            switch (events_->WaitAny()) {
            case kEventIndexNewPlay:
                EventWorkerNewPlay();
                break;
            case kEventIndexReadOrEnd:
                EventWorkerReadOrEnd();
                break;
            case kEventIndexStopAll:
                EventWorkerStopAll();
                break;
            case kEventIndexCallback:
                EventWorkerCallback();
                break;
            case kEventIndexSetVolumn:
                EventWorkerSetVolumn();
                break;
            case kEventIndexExit:
                return;
            default:
                break;
            }
        }
    }
    void EventWorkerNewPlay();
    void EventWorkerReadOrEnd();
    void EventWorkerStopAll();
    void EventWorkerCallback();
    void EventWorkerSetVolumn();
};  //PlayerImpl

void PlayerImpl::EventWorkerNewPlay() {
    std::vector<std::unique_ptr<CallbackData>> callbacks;
    std::vector<std::unique_ptr<PlayData>> plays;
    std::scoped_lock lock(mtx_pd_new_);
    for (auto& kv : pd_new_) {
        auto& pd = kv.second;
        LOG("New PlayID: %d, FileName: %s", pd->play_id, pd->file_name.c_str());
        auto new_play = std::make_unique<PlayData>(pd->play_id, pd->callback);
        new_play->decoder = GetDecoderByFilename(pd->file_name);
        if (!new_play->decoder || !new_play->decoder->Open(pd->file_name.c_str())) {
            LOG("Open file failed!");
            auto ended = std::make_unique<CallbackData>(pd->play_id, StopType::Error, pd->callback);
            callbacks.push_back(std::move(ended));
            continue;
        }
        LOG("Sound File Info:\n"
            "    Filename       : %s\n"
            "    Channels       : %d\n"
            "    SamplesPerSec  : %d\n"
            "    AvgBytesPerSec : %d\n"
            "    BlockAlign     : %d\n"
            "    BitsPerSample  : %d\n"
            "    TotalSamples   : %d\n"
            "    LengthInSeconds: %.3f",
            pd->file_name.c_str(), new_play->decoder->GetWaveFormat().channels,
            new_play->decoder->GetWaveFormat().samples_per_sec, new_play->decoder->GetWaveFormat().avg_bytes_per_sec,
            new_play->decoder->GetWaveFormat().block_align, new_play->decoder->GetWaveFormat().bits_per_sample,
            new_play->decoder->SamplesTotal(),
            static_cast<double>(new_play->decoder->SamplesTotal()) / new_play->decoder->GetWaveFormat().samples_per_sec);

        new_play->buffer = SoundBuffer::CreateSoundBuffer(pDS_, new_play->decoder->GetWaveFormat());
        if (!new_play->buffer) {
            LOG("Create sound buffer failed!");
            auto ended = std::make_unique<CallbackData>(pd->play_id, StopType::Error, pd->callback);
            callbacks.push_back(std::move(ended));
            continue;
        }
        LOG("Sound Buffer Created:\n"
            "    Num of Buffers    : %d\n"
            "    Samples per buffer: %d\n"
            "    Total Samples     : %d",
            new_play->buffer->GetBuffersNum(), new_play->buffer->GetSamplesSingleBuffer(),
            new_play->buffer->GetSamplesAllBuffers());

        std::size_t end_pos = new_play->decoder->SamplesTotal() % new_play->buffer->GetSamplesAllBuffers();
        std::vector<std::size_t> pos{ end_pos };
        for (std::size_t i = 0; i < new_play->buffer->GetBuffersNum(); i++) {
            pos.push_back(i * new_play->buffer->GetSamplesSingleBuffer() + 2);
        }
        new_play->buffer->AddPositionsEvent(events_->GetRawEvent(kEventIndexReadOrEnd), pos);
        new_play->buffer_index = new_play->buffer->GetBuffersNum() - 1;
        {
            std::scoped_lock lock_volume(mtx_volumn_);
            new_play->buffer->SetVolume(volume_);
        }
        plays.push_back(std::move(new_play));
    }
    if (!plays.empty()) {
        std::scoped_lock lock_playing(mtx_pd_playing_);
        for (auto& play : plays) {
            pd_playing_[play->play_id] = std::move(play);
        }
        events_->Set(kEventIndexReadOrEnd);
    }
    if (!callbacks.empty()) {
        std::scoped_lock lock_callback(mtx_pd_callback_);
        for (auto& callback : callbacks) {
            pd_callback_.push_back(std::move(callback));
        }
        events_->Set(kEventIndexCallback);
    }
    pd_new_.clear();
}
void PlayerImpl::EventWorkerReadOrEnd() {
    std::vector<std::unique_ptr<CallbackData>> callbacks;
    {
        std::scoped_lock lock(mtx_pd_playing_);
        std::vector<decltype(pd_playing_.begin())> to_erase;
        for (auto it = pd_playing_.begin(); it != pd_playing_.end(); ++it) {
            auto& pd = it->second;
            auto pos = pd->buffer->GetPosition();
            auto buff_size = pd->buffer->GetSamplesSingleBuffer();

            if (pd->stop_soon) {
                if (pos >= pd->end_pos || pos / buff_size != pd->end_pos / buff_size) {
                    auto ended = std::make_unique<CallbackData>(pd->play_id, StopType::PlayEnd, pd->callback);
                    callbacks.push_back(std::move(ended));
                    to_erase.push_back(it);
                }
            } else if (pos / buff_size == pd->buffer_index || !pd->playing) {
                if (pd->end_pos != kNotEnd) {
                    pd->stop_soon = true;
                    continue;
                }
                pd->buffer_index++;
                if (pd->buffer_index >= pd->buffer->GetBuffersNum()) {
                    pd->buffer_index = 0;
                }
                auto write_buffer = pd->buffer->GetBufferForWrite(pd->buffer_index);
                if (!write_buffer) {
                    auto ended = std::make_unique<CallbackData>(pd->play_id, StopType::Error, pd->callback);
                    callbacks.push_back(std::move(ended));
                    to_erase.push_back(it);
                } else {
                    auto read = pd->decoder->Read(write_buffer->Get(), buff_size);
                    if (read < buff_size) {
                        pd->end_pos = read + pd->buffer_index * buff_size;
                    }
                    if (!pd->playing) {
                        pd->playing = true;
                        pd->buffer->Play();
                    }
                }
            }
        }
        if (!to_erase.empty()) {
            for (auto it : to_erase) {
                pd_playing_.erase(it);
            }
        }
    }
    if (!callbacks.empty()) {
        std::scoped_lock lock(mtx_pd_callback_);
        for (auto& callback : callbacks) {
            pd_callback_.push_back(std::move(callback));
        }
        events_->Set(kEventIndexCallback);
    }
}
void PlayerImpl::EventWorkerStopAll() {
    std::vector<std::unique_ptr<CallbackData>> callbacks;
    {
        std::scoped_lock lock(mtx_pd_playing_, mtx_pd_new_);
        for (auto& kv : pd_new_) {
            auto& pd = kv.second;
            auto ended = std::make_unique<CallbackData>(pd->play_id, StopType::ForceStop, pd->callback);
            callbacks.push_back(std::move(ended));
        }
        for (auto& kv : pd_playing_) {
            auto& pd = kv.second;
            pd->decoder->Close();
            pd->buffer->Stop();
            auto ended = std::make_unique<CallbackData>(pd->play_id, StopType::ForceStop, pd->callback);
            callbacks.push_back(std::move(ended));
        }
        pd_new_.clear();
        pd_playing_.clear();
    }
    if (!callbacks.empty()) {
        std::scoped_lock lock(mtx_pd_callback_);
        for (auto& callback : callbacks) {
            pd_callback_.push_back(std::move(callback));
        }
        events_->Set(kEventIndexCallback);
    }
}
void PlayerImpl::EventWorkerCallback() {
    std::scoped_lock lock(mtx_pd_callback_);
    for (const auto& pd : pd_callback_) {
        if (pd->callback) {
            pd->callback(pd->play_id, pd->stop_type);
        }
    }
    pd_callback_.clear();
}
void PlayerImpl::EventWorkerSetVolumn() {
    std::scoped_lock lock(mtx_volumn_, mtx_pd_playing_);;
    for (const auto& kv : pd_playing_) {
        const auto& pd = kv.second;
        pd->buffer->SetVolume(volume_);
    }
}
}  // namespace

std::unique_ptr<player::Player> player::Player::GetPlayer(void** ppDS, void* hwnd) {
    if (!ppDS) {
        return nullptr;
    }
    if (!*ppDS) {
        if (!utils::CreateDSound(ppDS, hwnd)) {
            return nullptr;
        }
    }
    if (!player::Decoder::InitAllDecoders()) {
        return nullptr;
    }
    auto player = std::make_unique<PlayerImpl>(*ppDS);
    return player->IsValid() ? std::move(player) : nullptr;
}
