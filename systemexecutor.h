#ifndef SYSTEMEXECUTOR_H
#define SYSTEMEXECUTOR_H
#include <iostream>
#include <fstream>

#include <isystemexecutor.h>
#include "systemexecutionexception.h"

using namespace std;

class SystemExecutor : public ISystemExecutor {
private:
    int last_exit_code = 0;

public:
    string execute_command(const string& command) override;
    int get_last_exit_code() const override { return last_exit_code; }
};

#endif // SYSTEMEXECUTOR_H


