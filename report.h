#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>
#include "utils.h"

class Report
{
public:
    Report(){
        //filename = "Report_" + utils::currentDateTime();
    }
    std::string filename;

    // Filter by size
    std::vector<int> vlengths, hlengths, rowtypes, coltypes;
    std::vector<int> slopes; // filtering vlengths
    std::vector<int> yIntercepts; // filtering vlengths

};

#endif // REPORT_H
