#include "utils.h"

namespace TaskUtils {

    string generate_uuid() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 15);

        const char* chars = "0123456789abcdef";
        string uuid = "at_";

        for (int i = 0; i < 8; ++i) {
            uuid += chars[dis(gen)];
        }

        return uuid;
    }

    bool is_valid_tag_name(const string& tag) {
        if (tag.empty() || tag.length() > 50) {
            return false;
        }

        // Только буквы, цифры, подчеркивания и дефисы
        regex tag_regex("^[a-zA-Z0-9_-]+$");
        return regex_match(tag, tag_regex);
    }

    vector<string> split_string(const string& str, char delimiter) {
        vector<string> tokens;
        stringstream ss(str);
        string token;

        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    chrono::system_clock::time_point stringToTimePoint(const string& timeStr) {


        std::tm tm = {};
        std::istringstream ss(timeStr);

        // Парсим строку в соответствии с форматом "HH:mm dd.MM.yyyy"
        ss >> get_time(&tm, "%H:%M %d.%m.%Y");

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse time string");
        }

        // Преобразуем std::tm в time_t (учитываем локальное время)
        std::time_t tt = std::mktime(&tm);
        if (tt == -1) {
            throw std::runtime_error("Failed to convert tm to time_t");
        }

        // Преобразуем time_t в system_clock::time_point
        return std::chrono::system_clock::from_time_t(tt);
    }

    std::string timePointToString(const std::chrono::system_clock::time_point& tp) {
        // Преобразуем time_point в time_t
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);

        // Преобразуем time_t в tm (локальное время)
        std::tm tm = *std::localtime(&tt); // Безопасная версия с localtime_s на Windows

        // Форматируем tm в строку
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M %d.%m.%Y");

        return oss.str();
    }

    std::string timeFullPointToString(const std::chrono::system_clock::time_point& tp) {
        // Преобразуем time_point в time_t
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);

        // Преобразуем time_t в tm (локальное время)
        std::tm tm = *std::localtime(&tt); // Безопасная версия с localtime_s на Windows

        // Форматируем tm в строку
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S %d.%m.%Y");

        return oss.str();
    }
}
