#include "Logger.hpp"

Logger::Logger() {
    const std::string logdir = "logs";
    if (access(logdir.c_str(), F_OK) == -1) {
        mkdir(logdir.c_str(), 0755);
    }

    // Generate filename: logs/Log_<DD-MM-YYYY_HH:MM:SS>.log using strftime
    std::time_t t = std::time(NULL);
    std::tm* tm = std::localtime(&t);
    char timebuf[32];
    std::strftime(timebuf, sizeof(timebuf), "%d-%m-%Y_%H:%M:%S", tm);

    std::ostringstream oss;
    oss << logdir << "/Log_" << timebuf << ".log";
    logfilename = oss.str();

    logfile.open(logfilename.c_str(), std::ios::out | std::ios::app);
    if (!logfile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logfilename);
    }
}

Logger::~Logger() {
    if (logfile.is_open()) {
        logfile.close();
    }
}