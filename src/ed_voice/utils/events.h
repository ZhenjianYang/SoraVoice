#ifndef __UTILS_EVENTS_H__
#define __UTILS_EVENTS_H__

#include <memory>

namespace utils {
using RawEvent = void*;
class Events {
public:
    using WaitResult = unsigned;
    static constexpr int kMaxEvents = 32;

    virtual WaitResult WaitAny() const = 0;
    virtual WaitResult WaitAll() const = 0;
    virtual WaitResult WaitOne(int index) const = 0;
    virtual bool Set(int index) const = 0;
    virtual bool Reset(int index) const = 0;

    virtual RawEvent GetRawEvent(int index) const = 0;

    virtual ~Events() = default;
public:
    static std::unique_ptr<Events> CreateEvents(int num);
};
}  // namespace utils

#endif  // __UTILS_EVENTS_H__
