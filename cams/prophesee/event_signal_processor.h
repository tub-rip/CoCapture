#ifdef ENABLE_METAVISION_SDK
#ifndef RIP_COCAPTURE_GUI_EVENT_SIGNAL_PROCESSOR_H
#define RIP_COCAPTURE_GUI_EVENT_SIGNAL_PROCESSOR_H

#include <metavision/sdk/driver/camera.h>
#include <metavision/hal/facilities/i_antiflicker_module.h>
#include <metavision/hal/facilities/i_event_trail_filter_module.h>
#include <metavision/hal/facilities/i_erc_module.h>

// Only for Gen4.1 and IMX636 sensors!

namespace rcg::cams::prophesee {

    const char* const AFK_MODE_BAND_PASS = "Band Pass";
    const char* const AFK_MODE_BAND_STOP = "Band Stop";

    const char* const ETF_TYPE_TRAIL = "TRAIL";
    const char* const ETF_TYPE_STC_CUT_TRAIL = "STC-CUT-TRAIL";
    const char* const ETF_TYPE_STC_KEEP_TRAIL = "STC-KEEP-TRAIL";

    class EventSignalProcessor {
    public:
        EventSignalProcessor(Metavision::Camera* camera) :
            camera_                   (camera),
            anti_flicker_module_      (camera_->get_device().get_facility<Metavision::I_AntiFlickerModule>()),
            event_trail_filter_module_(camera_->get_device().get_facility<Metavision::I_EventTrailFilterModule>()),
            erc_module_               (camera_->get_device().get_facility<Metavision::I_ErcModule>()) {}

        ~EventSignalProcessor() {}

    // Anti-Flicker (AFK)
    public:
        // Enable & Disable
        bool AFK_Enable() {
            return anti_flicker_module_->enable(true);
        }

        bool AFK_Disable() {
            return anti_flicker_module_->enable(false);
        }

        bool AFK_IsEnabled() {
            return anti_flicker_module_->is_enabled();
        }

        // Filtering Mode
        std::string AFK_GetFilteringMode() {
            if(anti_flicker_module_->get_filtering_mode() == Metavision::I_AntiFlickerModule::AntiFlickerMode::BAND_PASS) {
                return std::string {AFK_MODE_BAND_PASS};
            } else if(anti_flicker_module_->get_filtering_mode() == Metavision::I_AntiFlickerModule::AntiFlickerMode::BAND_STOP) {
                return std::string {AFK_MODE_BAND_STOP};
            } else {
                return {};
            }
        }

        bool AFK_SetFilteringMode(const char* filtering_mode) {
            std::string filtering_mode_str {filtering_mode};
            if(filtering_mode_str == AFK_MODE_BAND_PASS) {
                return anti_flicker_module_->set_filtering_mode(Metavision::I_AntiFlickerModule::AntiFlickerMode::BAND_PASS);
            } else if(filtering_mode_str == AFK_MODE_BAND_STOP) {
                return anti_flicker_module_->set_filtering_mode(Metavision::I_AntiFlickerModule::AntiFlickerMode::BAND_STOP);
            } else {
                return false;
            }
        }

        // Frequency Band
        std::pair<int, int> AFK_GetFrequencyBand() {
            return anti_flicker_module_->get_frequency_band();
        }

        bool AFK_SetFrequencyBand(std::pair<int, int> frequency_band_Hz) {
            return anti_flicker_module_->set_frequency_band(frequency_band_Hz.first, frequency_band_Hz.second);
        }

        std::pair<int, int> AFK_GetMaxFrequencyBand() {
            return {anti_flicker_module_->get_min_supported_frequency(), anti_flicker_module_->get_max_supported_frequency()};
        }

        // Duty Cycle
        int AFK_GetDutyCycle() {
            return anti_flicker_module_->get_duty_cycle();
        }

        bool AFK_SetDutyCycle(int duty_cycle_percentage) {
            return anti_flicker_module_->set_duty_cycle(duty_cycle_percentage);
        }

        std::pair<int, int> AFK_GetDutyCycleRange() {
            return {anti_flicker_module_->get_min_supported_duty_cycle(), anti_flicker_module_->get_max_supported_duty_cycle()};
        }

        // Start Threshold
        int AFK_GetStartThreshold() {
            return anti_flicker_module_->get_start_threshold();
        };

        bool AFK_SetStartThreshold(int start_threshold) {
            return anti_flicker_module_->set_start_threshold(start_threshold);
        }

        std::pair<int, int> AFK_GetStartThresholdRange() {
            return {anti_flicker_module_->get_min_supported_start_threshold(), anti_flicker_module_->get_max_supported_start_threshold()};
        }

        // Stop Threshold
        int AFK_GetStopThreshold() {
            return anti_flicker_module_->get_stop_threshold();
        }

        bool AFK_SetStopThreshold(int stop_threshold) {
            return anti_flicker_module_->set_stop_threshold(stop_threshold);
        }

        std::pair<int, int> AFK_GetStopThresholdRange() {
            return {anti_flicker_module_->get_min_supported_stop_threshold(), anti_flicker_module_->get_max_supported_stop_threshold()};
        }

    // Event Trail Filter (STC/Trail)
    public:
        // Enable & Disable
        bool ETF_Enable() {
            return event_trail_filter_module_->enable(true);
        }

        bool ETF_Disable() {
            return event_trail_filter_module_->enable(false);
        }

        bool ETF_IsEnabled() {
            return event_trail_filter_module_->is_enabled();
        }

        // Filter Type
        std::vector<std::string> ETF_GetAvailableFilterTypes() {
            std::vector<std::string> available_filter_types;
            for(const auto& available_filter_type : event_trail_filter_module_->get_available_types()) {
                switch(available_filter_type) {
                    case Metavision::I_EventTrailFilterModule::Type::TRAIL:
                        available_filter_types.push_back(ETF_TYPE_TRAIL);
                        break;

                    case Metavision::I_EventTrailFilterModule::Type::STC_CUT_TRAIL:
                        available_filter_types.push_back(ETF_TYPE_STC_CUT_TRAIL);
                        break;

                    case Metavision::I_EventTrailFilterModule::Type::STC_KEEP_TRAIL:
                        available_filter_types.push_back(ETF_TYPE_STC_KEEP_TRAIL);
                }
            }

            return available_filter_types;
        }

        std::string ETF_GetFilterType() {
            switch(event_trail_filter_module_->get_type()) {
                case Metavision::I_EventTrailFilterModule::Type::TRAIL:
                    return ETF_TYPE_TRAIL;

                case Metavision::I_EventTrailFilterModule::Type::STC_CUT_TRAIL:
                    return ETF_TYPE_STC_CUT_TRAIL;

                case Metavision::I_EventTrailFilterModule::Type::STC_KEEP_TRAIL:
                    return ETF_TYPE_STC_KEEP_TRAIL;
            }

            return {};
        }

        bool ETF_SetFilterType(std::string filter_type) {
            if(filter_type == ETF_TYPE_TRAIL) {
                return event_trail_filter_module_->set_type(Metavision::I_EventTrailFilterModule::Type::TRAIL);
            } else if(filter_type == ETF_TYPE_STC_CUT_TRAIL) {
                return event_trail_filter_module_->set_type(Metavision::I_EventTrailFilterModule::Type::STC_CUT_TRAIL);
            } else if(filter_type == ETF_TYPE_STC_KEEP_TRAIL) {
                return event_trail_filter_module_->set_type(Metavision::I_EventTrailFilterModule::Type::STC_KEEP_TRAIL);
            }

            return false;
        }

        // Filter Threshold Delay
        int ETF_GetFilterThresholdDelay() {
            return event_trail_filter_module_->get_threshold();
        }

        bool ETF_SetFilterThresholdDelay(int filter_threshold_delay_us) {
            return event_trail_filter_module_->set_threshold(filter_threshold_delay_us);
        }

        std::pair<int, int> ETF_GetFilterThresholdDelayRange() {
            return {event_trail_filter_module_->get_min_supported_threshold(), event_trail_filter_module_->get_max_supported_threshold()};
        }

    // Event Rate Controller (ERC)
    public:
        // Enable & Disable
        bool ERC_Enable() {
            return erc_module_->enable(true);
        }

        bool ERC_Disable() {
            return erc_module_->enable(false);
        }

        bool ERC_IsEnabled() {
            return erc_module_->is_enabled();
        }

        // CD Event Rate
        int ERC_GetCDEventRate() {
            return erc_module_->get_cd_event_rate() / std::pow(10, 6);
        }

        bool ERC_SetCDEventRate(int cd_event_rate_MEv) {
            return erc_module_->set_cd_event_rate(cd_event_rate_MEv * std::pow(10, 6));
        }

        std::pair<int, int> ERC_GetCDEventRateRange() {
            return {erc_module_->get_min_supported_cd_event_rate() / std::pow(10, 6), erc_module_->get_max_supported_cd_event_rate() / std::pow(10, 6)};
        }

    private:
        Metavision::Camera* camera_;

        // Anti-Flicker (AFK)
        Metavision::I_AntiFlickerModule* anti_flicker_module_;

        // Event Trail Filter (STC/Trail)
        Metavision::I_EventTrailFilterModule* event_trail_filter_module_;

        // Event Rate Controller (ERC)
        Metavision::I_ErcModule* erc_module_;
    };

} // rcg::cams::prophesee

#endif //RIP_COCAPTURE_GUI_EVENT_SIGNAL_PROCESSOR_H
#endif //ENABLE_METAVISION_SDK