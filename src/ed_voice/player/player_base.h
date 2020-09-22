#ifndef __PLAYER_PLAYER_BASE_H__
#define __PLAYER_PLAYER_BASE_H__

#include <cstdint>

namespace player {
struct WaveFormat {
    std::uint16_t format_tag;        // format type
    std::uint16_t channels;          // number of channels (i.e. mono, stereo, etc.)
    std::uint32_t samples_per_sec;   // sample rate
    std::uint32_t avg_bytes_per_sec; // for buffer estimation
    std::uint16_t block_align;       // block size of data
    std::uint16_t bits_per_sample;   // number of bits per sample of mono data
};  //WaveFormat
using BuffByte = char;
constexpr int kVolumeMax = 100;
}  // namespace player

#endif  // __PLAYER_PLAYER_BASE_H__