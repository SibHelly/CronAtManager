#include "systemexecutor.h"

string SystemExecutor::execute_command(const string& command){
    string result;
    char buffer[128];

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw SystemExecutionException("Failed to execute command: " + command);
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    last_exit_code = pclose(pipe);
    return result;
}
