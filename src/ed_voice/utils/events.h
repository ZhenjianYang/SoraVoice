#ifndef __UTILS_EVENTS_H__
#define __UTILS_EVENTS_H__

#include <memory>

namespace utils {
using RawEvent = void*;
class Events {
public:
    using WaitResult = unsigned;
    static constexpr std::size_t kMaxEvents = 32;

    virtual WaitResult WaitAny() const = 0;
    virtual WaitResult WaitAll() const = 0;
    virtual WaitResult WaitOne(std::size_t index) const = 0;
    virtual bool Set(std::size_t index) const = 0;
    virtual bool Reset(std::size_t index) const = 0;

    virtual RawEvent GetRawEvent(std::size_t index) const = 0;

    virtual ~Events() = default;
public:
    static std::unique_ptr<Events> CreateEvents(std::size_t num);
};
}  // namespace utils

#endif  // __UTILS_EVENTS_H__
