#ifndef CONF_H
#define CONF_H

#include <string>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>  // Для mkdir()
#include <filesystem>   // Для работы с путями (C++17)

std::string getConfigPath();

#endif // CONF_H
