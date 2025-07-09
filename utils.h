#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <regex>
#include <iomanip>

using namespace std;

namespace TaskUtils{
    string generate_uuid();
    bool is_valid_tag_name(const string& tag);
    vector<string> split_string(const string& str, char delimiter);
    chrono::system_clock::time_point stringToTimePoint(const string& timeStr);
    string timePointToString(const std::chrono::system_clock::time_point& tp);
    string timeFullPointToString(const std::chrono::system_clock::time_point& tp);
}
#endif // UTILS_H
