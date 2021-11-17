#include <signal.h>
#include <iostream>
#include <libconfig.h++>
#include <glog/logging.h>
#include <chrono>
#include <thread>
#include <filesystem>

#include "tools.h"

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

    int camID = cfg_data["camID"];
    cv::Size boardSize(cfg_data["calibration"]["width"], cfg_data["calibration"]["height"]);
    cv::VideoCapture cap(camID);

    cv::Mat img;
    std::vector<cv::Point2f> pointbuf;
    bool found = false;

    for (const auto & entry : std::filesystem::directory_iterator(results_path)) {
        img = cv::imread(entry.path());
        if(img.empty()) {
            // LOG(ERROR) << "img "<< entry.path() << " not an image ";
            continue;
        }
        found = cv::findCirclesGrid(img, boardSize, pointbuf);
        if(found) {
            cv::drawChessboardCorners(img, boardSize, cv::Mat(pointbuf), true);
        }
        cv::imshow("Image View", img);
        cv::waitKey(0);
    }

    std::string outputFilename = results_path + "matrix.txt";
    // runAndSave(outputFilename, imagePoints, imageSize,
    //            boardSize, pattern, squareSize, grid_width, release_object, aspectRatio,
    //            flags, cameraMatrix, distCoeffs,
    //            writeExtrinsics, writePoints, writeGrid);
    return 0;
}