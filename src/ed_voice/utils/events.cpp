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
    WaitResult WaitOne(int index) const override {
        auto wait = index < num_ ? WaitForSingleObject(events_[index], INFINITE)
                                 : WAIT_FAILED;
        return static_cast<WaitResult>(wait);
    }
    bool Set(int index) const override {
        if (index >= num_) {
            return false;
        }
        return SetEvent(events_[index]);
    }
    bool Reset(int index) const override {
        if (index >= num_) {
            return false;
        }
        return ResetEvent(events_[index]);
    }

    RawEvent GetRawEvent(int index) const override {
        if (index >= num_) {
            return nullptr;
        }
        return reinterpret_cast<RawEvent>(events_[index]);
    }

    bool IsValid() const {
        return valid_;
    }

    explicit EventsImpl(int num)
        : num_{ num }, events_{ std::make_unique<HANDLE[]>(num) } {
        for (int i = 0; i < num_; i++) {
            events_[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (!events_[i]) {
                return;
            }
        }
        valid_ = true;
    }
    ~EventsImpl() override {
        for (int i = 0; i < num_; i++) {
            if (events_[i]) {
                CloseHandle(events_[i]);
                events_[i] = NULL;
            }
        }
    }
private:
    std::unique_ptr<HANDLE[]> events_;
    const int num_;
    bool valid_ = false;

    EventsImpl(const EventsImpl&) = delete;
    EventsImpl& operator=(const EventsImpl&) = delete;
};  // EventsImpl
}  // namespace

std::unique_ptr<Events> utils::Events::CreateEvents(int num) {
    if (num > kMaxEvents) {
        return nullptr;
    }
    auto events = std::make_unique<EventsImpl>(num);
    return events->IsValid() ? std::move(events) : nullptr;
}
