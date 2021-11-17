#include <signal.h>
#include <iostream>
#include <libconfig.h++>
#include <glog/logging.h>
#include <chrono>
#include <thread>

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

    std::string results_path = "results/" + date2Str();
    mkdirp(results_path);

    int camID = cfg_data["camID"];
    cv::Size boardSize(cfg_data["board_width"], cfg_data["board_height"]);
    cv::VideoCapture cap(camID);

    cv::Mat img, view;
    std::vector<cv::Point2f> pointbuf;
    int count = 0;
    bool found = false;

    while(1) {
        cap >> img;
        img.copyTo(view);
        found = cv::findCirclesGrid(view, boardSize, pointbuf);
        if(found) {
            cv::drawChessboardCorners(view, boardSize, cv::Mat(pointbuf), true);
        }
        cv::imshow("Image View", view);
        char c = (char)cv::waitKey();

        if( c == 27 || c == 'q' || c == 'Q' ) {
            break;
        } else if( (c == 's') && found) {
            std::string save_path = results_path + "/" + std::to_string(count) + ".png";
            std::cout << "Saving Frame["<< count << "] at " << save_path << '\n';
            cv::imwrite(save_path, img);
            count++;
        }
    }
    return 0;
}