#include "event_queue.h"

namespace event_queue {

    void EventQueue::queue_events(const Metavision::EventCD *begin,
                                  const Metavision::EventCD *end) {
        std::lock_guard<std::mutex> lock(m_);
        for (auto it = begin; it != end; ++it) {
            queue_.push(*it);
        }
    }

} // event_queue