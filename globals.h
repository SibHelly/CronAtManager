#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>  // Для mkdir()
#include <filesystem>   // Для работы с путями (C++17)

static std::string getConfigPath() {
    const char* homeDir = getenv("HOME");

    // Если переменная HOME не установлена, получаем из /etc/passwd
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        if (pw) homeDir = pw->pw_dir;
    }

    if (!homeDir) {
        throw std::runtime_error("Не удалось определить домашнюю директорию");
    }

    // Формируем полный путь
    std::string configPath = std::string(homeDir) + "/config/task-manager/";

    // Создаем директории, если они не существуют
    std::string command = "mkdir -p " + configPath;
    if (system(command.c_str()) != 0) {
        throw std::runtime_error("Ошибка создания директории: " + configPath);
    }

    return configPath;
}

#endif // GLOBALS_H
