#ifndef GLOBALS_H
#define GLOBALS_H
#include <iostream>
#include <cstdlib>
#include <stdexcept>
using namespace std;

const string config_directory = "~/task_scheduler/";

static std::string getConfigPath() {
    const char* home = getenv("HOME");
    if (!home) {
        throw std::runtime_error("Cannot determine home directory");
    }
    return std::string(home) + "/task_scheduler/";
}
#endif // GLOBALS_H
