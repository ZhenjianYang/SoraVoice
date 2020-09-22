#include "events.h"

#include <Windows.h>

namespace {
using utils::Events;
using utils::RawEvent;

class EventsImpl : public Events {
public:
    WaitResult WaitAny() const override {
        auto wait = WaitForMultipleObjects(num_, events_.get(), FALSE, INFINITE);
        return static_cast<WaitResult>(wait);
    }
    WaitResult WaitAll() const override {
        auto wait = WaitForMultipleObjects(num_, events_.get(), TRUE, INFINITE);
        return static_cast<WaitResult>(wait);
    }
    WaitResult WaitOne(std::size_t index) const override {
        auto wait = index < num_ ? WaitForSingleObject(events_[index], INFINITE)
                                 : WAIT_FAILED;
        return static_cast<WaitResult>(wait);
    }
    bool Set(std::size_t index) const override {
        if (index >= num_) {
            return false;
        }
        return SetEvent(events_[index]);
    }
    bool Reset(std::size_t index) const override {
        if (index >= num_) {
            return false;
        }
        return ResetEvent(events_[index]);
    }

    RawEvent GetRawEvent(std::size_t index) const override {
        if (index >= num_) {
            return nullptr;
        }
        return reinterpret_cast<RawEvent>(events_[index]);
    }

    bool IsValid() const {
        return valid_;
    }

    explicit EventsImpl(std::size_t num)
        : num_{ num }, events_{ std::make_unique<HANDLE[]>(num) } {
        for (std::size_t i = 0; i < num_; i++) {
            events_[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (!events_[i]) {
                return;
            }
        }
        valid_ = true;
    }
    ~EventsImpl() override {
        for (std::size_t i = 0; i < num_; i++) {
            if (events_[i]) {
                CloseHandle(events_[i]);
                events_[i] = NULL;
            }
        }
    }
private:
    std::unique_ptr<HANDLE[]> events_;
    const std::size_t num_;
    bool valid_ = false;

    EventsImpl(const EventsImpl&) = delete;
    EventsImpl& operator=(const EventsImpl&) = delete;
};  // EventsImpl
}  // namespace

std::unique_ptr<Events> utils::Events::CreateEvents(std::size_t num) {
    if (num > kMaxEvents) {
        return nullptr;
    }
    auto events = std::make_unique<EventsImpl>(num);
    return events->IsValid() ? std::move(events) : nullptr;
}
