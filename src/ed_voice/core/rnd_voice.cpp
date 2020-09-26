#include "rnd_voice.h"

#include <random>
#include <ctime>

namespace {
constexpr char* kRndVoiceList[] = {
    "data/se/ed7v2665.wav",
    "data/se/ed7v2666.wav",
    "data/se/ed7v2705.wav",
    "data/se/ed7v2706.wav",
    "data/se/ed7v2747.wav",
    "data/se/ed7v2748.wav",
    "data/se/ed7v2902.wav",
    "data/se/ed7v2903.wav",
    "data/se/ed7v2947.wav",
    "data/se/ed7v2948.wav",
    "data/se/ed7v3241.wav",
    "data/se/ed7v3242.wav",
    "data/se/ed7v3304.wav",
    "data/se/ed7v3305.wav",
    "data/se/ed7v3382.wav",
    "data/se/ed7v3383.wav",
    "data/se/ed7v3441.wav",
    "data/se/ed7v3442.wav",
    "data/se/ed7v3506.wav",
    "data/se/ed7v3507.wav",
    "data/se/ed7v3562.wav",
    "data/se/ed7v3563.wav",
    "data/se/ed7v3591.wav",
    "data/se/ed7v3592.wav",
    "data/se/ed7v3699.wav",
    "data/se/ed7v3700.wav",
    "data/se/ed7v3772.wav",
    "data/se/ed7v3773.wav",
    "data/se/ed7v3828.wav",
    "data/se/ed7v3829.wav",
    "data/se/ed7v3883.wav",
    "data/se/ed7v3884.wav",
    "data/se/ed7v3934.wav",
    "data/se/ed7v3935.wav",
    "data/se/ed7v4040.wav",
    "data/se/ed7v4041.wav"
    };
}

const char* core::GetRandomVoice() {
    constexpr int num = std::size(kRndVoiceList);

    std::default_random_engine random((unsigned)std::time(nullptr));
    std::uniform_int_distribution<int> dist(0, num - 1);
    return kRndVoiceList[dist(random)];
}
