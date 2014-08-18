#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

namespace utils {
/*

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
// REF: http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
*/
} // end namespace utils
#endif // UTILS_H
