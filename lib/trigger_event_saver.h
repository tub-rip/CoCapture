#ifndef RIP_COCAPTURE_GUI_TRIGGER_EVENT_SAVER_H
#define RIP_COCAPTURE_GUI_TRIGGER_EVENT_SAVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <metavision/sdk/base/events/event_ext_trigger.h>


class TriggerEventSaver {
public:
    TriggerEventSaver() = default;

    TriggerEventSaver(const TriggerEventSaver& other) = delete;

    TriggerEventSaver(TriggerEventSaver&& other) noexcept
            : file_(std::move(other.file_)) {
        other.file_.setstate(std::ios_base::failbit);
    }

    TriggerEventSaver& operator=(const TriggerEventSaver& other) = delete;

    TriggerEventSaver& operator=(TriggerEventSaver&& other) noexcept {
        if (this != &other) {
            file_ = std::move(other.file_);
            other.file_.setstate(std::ios_base::failbit);
        }
        return *this;
    }

    ~TriggerEventSaver() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    void set_path(const std::string& path) {
        if (file_.is_open()) {
            file_.close();
        }
        std::string file_path = path + "_trigger_events.txt";
        file_.open(file_path, std::ios::out | std::ios::trunc);
    }

    void stop_writing() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    void save_events(const Metavision::EventExtTrigger *begin,
                     const Metavision::EventExtTrigger *end) {
        if (file_.is_open() && file_.good()) {
            for (const Metavision::EventExtTrigger *ev = begin; ev != end; ++ev) {
                std::cout << ev->t << " " << ev->id << " " << ev->p << "\n";
                file_ << ev->t << " " << ev->id << " " << ev->p << "\n";
            }
        } else {
            std::cerr << "Error: File is not open or not in a good state" << std::endl;
        }
    }

private:
    std::ofstream file_;
};

#endif //RIP_COCAPTURE_GUI_TRIGGER_EVENT_SAVER_H
