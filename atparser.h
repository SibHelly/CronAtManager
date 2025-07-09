#ifndef ATPARSER_H
#define ATPARSER_H

#include <iostream>
#include <vector>
#include <regex>
#include <memory>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "basicStructs.h"
#include "isystemexecutor.h"

using namespace std;
using namespace boost::algorithm;

class AtParser
{
public:
    // AtParser();
    static chrono::system_clock::time_point parse_at_timeSlash(const string& time_str);
    static vector<AtTask> parse_at_logs(const string& output);
    static vector<AtTask> parse_atq_output(const string& atq_output, unique_ptr<ISystemExecutor>& executor);
    static string format_at_time(const chrono::system_clock::time_point& time);
    static chrono::system_clock::time_point parse_at_time(const string& time_str);
};

#endif // ATPARSER_H
