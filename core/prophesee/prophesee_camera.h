#ifdef ENABLE_METAVISION_SDK
#ifndef COCAPTURE_PROPHESEE_CAMERA_H
#define COCAPTURE_PROPHESEE_CAMERA_H

#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>
#include <metavision/hal/facilities/i_device_control.h>
#include <metavision/hal/facilities/i_hw_identification.h>
#include <metavision/hal/facilities/i_ll_biases.h>
#include <metavision/hal/facilities/i_trigger_in.h>
#include <metavision/hal/facilities/i_erc.h>

#include "camera_interface.h"

namespace Cocapture {

    class PropheseeCamera : public ICamera {
    public:
        PropheseeCamera(std::string serial_number);

    public:
        void Start() override;
        std::string GetInfo(int info) override;
        double GetProp(int property) override;
        void SetProp(int property, double value) override;
        void OutputFrame(int, cv::Mat& image_frame) override;
        void StartRecording(std::string output_directory_path) override;
        void StopRecording() override;
        void Clean() override;

    public:
        static std::string ToString(int value);
        static int ToEnum(std::string value);

    private:
        struct BiasRanges {
            std::pair<int, int> bias_diff_off_range;
            std::pair<int, int> bias_diff_on_range;
            std::pair<int, int> bias_fo_range;
            std::pair<int, int> bias_hpf_range;
            std::pair<int, int> bias_refr_range;
        };

    private:
        class TriggerEventSaver {
        public:
            void Open(std::string trigger_events_file_path) {
                trigger_events_file_.open(trigger_events_file_path, std::ios::out);
            }

            void AddEvents(const Metavision::EventExtTrigger* begin, const Metavision::EventExtTrigger* end) {
                for(const Metavision::EventExtTrigger* ev = begin; ev != end; ev++) {
                    trigger_events_file_ << ev->t << " " << ev->id << " " << ev->p << "\n";
                }
            }

            void SaveCallbackId(size_t callback_id) { callback_id_ = callback_id; }

            size_t Close() {
                trigger_events_file_.close();
                return callback_id_;
            }

        private:
            std::fstream trigger_events_file_;
            size_t callback_id_;
        };

    private:
        void ReadBiasRanges(std::string file_name, std::string camera_generation);
        int GetSyncMode();
        void SetSyncMode(int synchronization_mode);

    private:
        Metavision::Camera camera_;
        std::unique_ptr<Metavision::CDFrameGenerator> cd_frame_generator_;

        cv::Mat cd_frame_;
        std::mutex cd_frame_mutex_;

        Metavision::I_HW_Identification* hw_identification_;
        Metavision::I_DeviceControl* device_control_;
        Metavision::I_TriggerIn* trigger_in_;
        Metavision::I_LL_Biases* biases_;

        BiasRanges bias_ranges_;
        TriggerEventSaver trigger_event_saver_;
    };

} // Cocapture

#endif //COCAPTURE_PROPHESEE_CAMERA_H
#endif //ENABLE_METAVISION_SDK