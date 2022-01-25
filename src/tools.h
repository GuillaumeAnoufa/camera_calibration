#include <iostream>
#include <glog/logging.h>
#include <sys/stat.h>
#include <iomanip>
#include <sstream>
#include <algorithm>

enum Pattern { CHESSBOARD = 0, CIRCLES_GRID = 1, ASYMMETRIC_CIRCLES_GRID = 2 };

std::string date2Str() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream res;
    res << std::setfill('0') << std::setw(2) << tm.tm_year + 1900;
    res << std::setfill('0') << std::setw(2) << tm.tm_mon + 1;
    res << std::setfill('0') << std::setw(2) << tm.tm_mday;
    res << "_" << std::setfill('0') << std::setw(2) << tm.tm_hour;
    res << std::setfill('0') << std::setw(2) << tm.tm_min;
    res << std::setfill('0') << std::setw(2) << tm.tm_sec;
    return res.str();
}

bool mkdirp(std::string& path, mode_t mode = 0777) {
    struct stat st;
    LOG(INFO) << "creating [" << path << "]";

    for (std::string::iterator iter = path.begin(); iter != path.end();) {
        std::string::iterator newIter = std::find(iter, path.end(), '/');
        std::string newPath = path;

        if (stat(newPath.c_str(), &st) != 0) {
            if (mkdir(newPath.c_str(), mode) != 0 && errno != EEXIST) {
                LOG(FATAL) << "cannot create folder [" << newPath << "] : " << strerror(errno);
                return false;
            }
        } else {
            if (!S_ISDIR(st.st_mode)) {
                errno = ENOTDIR;
                LOG(ERROR) << "path [" << newPath << "] not a dir";
                return false;
            } else {
                DVLOG(2) << "path [" << newPath << "]  already exists";
            }
            iter = newIter;
            if (newIter != path.end())
                ++iter;
        }
    }
    return true;
}