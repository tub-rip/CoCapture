#include "utils.h"

namespace utils {
    bool parse_comman_line(int argc, const char *argv[], Parameters &params) {
        const std::string program_desc("Recording and Visualization with the co-capture system.\n");
        std::string homography_path;
        po::options_description options_desc("Options");
        options_desc.add_options()
                ("help,h", "Produce help message.")
                ("warp", po::bool_switch(&params.do_warp)->default_value(false),
                 "Warp gray-scale frames according to a given homography")
                ("homography", po::value<std::string>(&homography_path)->default_value(""))
                ("overlay", po::bool_switch(&params.overlay)->default_value(false),
                 "Visualize event in overlay of grayscale frames")
                ("show_snr", po::bool_switch(&params.show_snr)->default_value(false),
                 "Show an estimate of the current SNR")
                ("roi", po::bool_switch(&params.roi)->default_value(false), "Show an estimate of the current SNR");

        po::variables_map vm;
        try {
            po::store(po::command_line_parser(argc, argv).options(options_desc).run(), vm);
            po::notify(vm);
        } catch (po::error &e) {
            MV_LOG_ERROR() << program_desc;
            MV_LOG_ERROR() << options_desc;
            MV_LOG_ERROR() << "Parsing error:" << e.what();
            return false;
        }

        if (vm.count("help")) {
            MV_LOG_INFO() << program_desc;
            MV_LOG_INFO() << options_desc;
            return false;
        }

        if (homography_path.empty()) {
            if (params.do_warp) {
                std::cout << "No homography provided, will proceed without warping" << std::endl;
                params.do_warp = false;
            }
        } else {
            // Load homography
            std::ifstream ifs;
            std::string line;
            ifs.open(homography_path, std::ios::in);
            if (ifs) {
                for (int i = 0; i < 3; ++i) {
                    std::getline(ifs, line);
                    std::stringstream ss(line);
                    for (int j = 0; j < 3; ++j) {
                        ss >> params.homography(i, j);
                    }
                }
            }
            ifs.close();
        }

        if (params.overlay && !params.do_warp) {
            throw std::invalid_argument("For overlay visualization warping must be activated by --warp!");
        }
        return true;
    }


    void set_roi(Metavision::Camera &cam) {
        auto roi_handle = cam.roi();
        roi_handle.set(std::vector<Metavision::Roi::Rectangle>{
                {0,   260, 30, 20},
                {320, 320, 10, 10}});
    }
}
