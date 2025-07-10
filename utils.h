#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <regex>
#include <iomanip>
#include <string>
#include <cctype>

using namespace std;

namespace TaskUtils{
    string generate_uuid();
    bool is_valid_tag_name(const string& tag);
    vector<string> split_string(const string& str, char delimiter);
    chrono::system_clock::time_point stringToTimePoint(const string& timeStr);
    string timePointToString(const std::chrono::system_clock::time_point& tp);
    string timeFullPointToString(const std::chrono::system_clock::time_point& tp);

    // Удаляет пробельные символы с начала строки (левый trim)
    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
    }

    // Удаляет пробельные символы с конца строки (правый trim)
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), s.end());
    }

    // Удаляет пробельные символы с обоих концов строки (полный trim)
    inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // Версия, которая возвращает обрезанную строку (а не изменяет оригинал)
    inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }
}
#endif // UTILS_H
