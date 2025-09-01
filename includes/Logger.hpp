#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>   // mkdir
#include <sys/types.h>  // mode_t
#include <unistd.h>     // access()

class Logger {
public:
    Logger();
    ~Logger();

    template<typename T>
    Logger& operator<<(const T& msg) {
        logfile << msg;
        logfile.flush();
        return *this;
    }

    // For manipulators like std::endl
    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(logfile);
        logfile.flush();
        return *this;
    }

private:
    std::string logfilename;
    std::ofstream logfile;

    Logger(const Logger &other);
    Logger& operator=(const Logger &other);
};

#endif