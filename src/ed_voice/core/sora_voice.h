#ifndef __CORE_SORA_VOICE_H__

#include <memory>
#include <string>
#include <vector>

struct SoraVoice {
public:
    virtual int Play(uint8_t*) = 0;
    virtual int Stop() = 0;
    
    virtual ~SoraVoice() = default;

    static std::unique_ptr<SoraVoice> GetSoraVoice(
            const std::string& tille, const std::string& built_date,
            std::vector<std::unique_ptr<char[]>>&& movable_strs);
};  // SoraVoice

#endif  // __CORE_SORA_VOICE_H__
