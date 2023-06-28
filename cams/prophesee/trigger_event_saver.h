#ifdef ENABLE_METAVISION_SDK
#ifndef RIP_COCAPTURE_GUI_TRIGGER_EVENT_SAVER_H
#define RIP_COCAPTURE_GUI_TRIGGER_EVENT_SAVER_H

#include <fstream>

#include <metavision/sdk/base/events/event_ext_trigger.h>

namespace rcg::cams::prophesee {

    class TriggerEventSaver {
    public:
        void OpenFile(const char* trigger_events_file) {
            trigger_events_file_.open(trigger_events_file, std::ios::out);
        }

        void AddEvents(const Metavision::EventExtTrigger* begin, const Metavision::EventExtTrigger* end) {
            for(const Metavision::EventExtTrigger* ev = begin; ev != end; ev++) {
                trigger_events_file_ << ev->t << " " << ev->id << " " << ev->p << "\n";
            }
        }

        void SetCallBackId(size_t callback_id) {
            callback_id_ = callback_id;
        }

        size_t GetCallBackId() {
            return callback_id_;
        }

        void CloseFile() {
            trigger_events_file_.close();
        }

    private:
        std::fstream trigger_events_file_;
        size_t callback_id_;
    };

} // rcg::cams::prophesee

#endif //RIP_COCAPTURE_GUI_TRIGGER_EVENT_SAVER_H
#endif //ENABLE_METAVISION_SDK