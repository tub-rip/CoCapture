#ifndef RIP_COCAPTURE_GUI_EVENT_QUEUE_H
#define RIP_COCAPTURE_GUI_EVENT_QUEUE_H

#include <queue>
#include <mutex>
#include <optional>

#include <metavision/sdk/base/events/event_cd.h>


namespace event_queue {

    class EventQueue {
    public:
        EventQueue() = default;

        EventQueue(const EventQueue &) = delete;

        EventQueue &operator=(const EventQueue &) = delete;

        EventQueue(EventQueue &&other) {
            queue_ = std::move(other.queue_);
        }

        ~EventQueue() = default;

        void queue_events(const Metavision::EventCD *begin,
                          const Metavision::EventCD *end);

        unsigned long size() const {
            std::lock_guard<std::mutex> lock(m_);
            return queue_.size();
        }

        std::optional<Metavision::EventCD> pop() {
            std::lock_guard<std::mutex> lock(m_);
            if (queue_.empty()) {
                return {};
            }
            Metavision::EventCD tmp = queue_.front();
            queue_.pop();;
            return tmp;
        }


    private:
        std::queue<Metavision::EventCD> queue_;
        mutable std::mutex m_;
    };

} // event_queue

#endif //RIP_COCAPTURE_GUI_EVENT_QUEUE_H
