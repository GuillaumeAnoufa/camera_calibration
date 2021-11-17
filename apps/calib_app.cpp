#include <signal.h>
#include <iostream>
#include <libconfig.h++>
#include <glog/logging.h>
#include <chrono>
#include <thread>
#include <filesystem>

#include "tools.h"
#include "calibration.h"

#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

static void sig_handler(int signo)
{
    switch (signo)
    {
        case SIGINT:  // Windows & Linux well defined (ctrl + V signal)
            LOG(ERROR) << "caught SIGINT",
            exit(0);
            break;
        default:
            LOG(ERROR) << "unexpected signal caught: ";
    }
}

int main(int argc, char* argv[])
{
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);
#if DEBUG
    FLAGS_logtostderr = 1;
#endif

    std::string configPath ="config.cfg";
    libconfig::Config cfg;
    try {
        cfg.readFile(configPath.c_str());
    } catch(const libconfig::FileIOException &fioex) {
        LOG(FATAL) << "Cannot read config file at: " << configPath;
    }
    const libconfig::Setting& cfg_data = cfg.getRoot()["info"];

    struct sigaction sa_new;

    sa_new.sa_handler = sig_handler;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_new, nullptr);

    std::string results_path = cfg_data["images_dir"];

    // Nombre de cercle en largeur et hauteur afin de former le plus grand rectangle
    cv::Size boardSize(cfg_data["board_width"], cfg_data["board_height"]);

    // Taille en cm entre deux cercles
    float squareSize = cfg_data["square_size"];
    float grid_width = squareSize * (boardSize.width - 1);

    cv::Size imageSize;
    std::vector<std::vector<cv::Point2f> > imagePoints;

    int flags = 0;

    flags |= CALIB_FIX_PRINCIPAL_POINT; // fix the principal point at the center

    // flags |= CALIB_ZERO_TANGENT_DIST; // assume zero tangential distortion

    for (const auto & entry : std::filesystem::directory_iterator(results_path)) {
        cv::Mat img;
        bool found = false;

        img = cv::imread(entry.path());
        if(img.empty()) {
            // LOG(ERROR) << "img "<< entry.path() << " not an image ";
            continue;
        }
        std::vector<cv::Point2f> pointbuf;
        found = cv::findCirclesGrid(img, boardSize, pointbuf);
        if(found) {
            cv::drawChessboardCorners(img, boardSize, cv::Mat(pointbuf), true);
        }
        imagePoints.push_back(pointbuf);
        imageSize = img.size();
        cv::imshow("Image View", img);
        cv::waitKey(1);
    }

    cv::Mat cameraMatrix, distCoeffs;
    bool writeExtrinsics = true;
    bool writePoints = true;
    bool writeGrid = true;

    std::string outputFilename = results_path + "/matrix.yaml";
    runAndSave(outputFilename, imagePoints, imageSize,
               boardSize, squareSize, grid_width,
               flags, cameraMatrix, distCoeffs,
               writeExtrinsics, writePoints, writeGrid);
    return 0;
}