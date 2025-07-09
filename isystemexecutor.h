#ifndef ISYSTEMEXECUTOR_H
#define ISYSTEMEXECUTOR_H
#include <iostream>

class ISystemExecutor {
public:
    virtual ~ISystemExecutor() = default;
    virtual std::string execute_command(const std::string& command) = 0;
    virtual int get_last_exit_code() const = 0;
};

#endif // ISYSTEMEXECUTOR_H
