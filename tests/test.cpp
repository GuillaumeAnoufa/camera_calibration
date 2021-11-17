#include <signal.h>
#include <iostream>
#include <libconfig.h++>
#include <glog/logging.h>
#include <chrono>
#include <thread>

static void sig_handler(int signo)
{
    switch (signo)
    {
        case SIGINT:  // Windows & Linux well defined (ctrl + V signal)
            LOG(WARNING) << "caught SIGINT",
            exit(0);
            break;
        default:
            LOG(ERROR) << "unexpected signal caught: ";
    }
}


int main(int argc, char* argv[])
{
    google::InitGoogleLogging(argv[0]);
    if(argc != 2) {
        std::cout << "Enter config file path " << argc<< '\n';
        return -1;
    }
    std::string configPath = argv[1];
    libconfig::Config cfg;
    try {
        cfg.readFile(configPath.c_str());
    } catch(const libconfig::FileIOException &fioex) {
        LOG(ERROR) << "Cannot read config file at: " << configPath ;
    }
    const libconfig::Setting& cfg_data = cfg.getRoot()["info"];

    struct sigaction sa_new;

    sa_new.sa_handler = sig_handler;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_new, nullptr);

    int count = 1;
    while(1) {
        std::cout << "Do stuff here["<< count << "]" << '\n';
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100));
        count++;
    }

    return 0;
}