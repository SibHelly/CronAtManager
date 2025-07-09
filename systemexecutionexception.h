#ifndef SYSTEMEXECUTIONEXCEPTION_H
#define SYSTEMEXECUTIONEXCEPTION_H
#include "taskschedulerexception.h"

class SystemExecutionException: public TaskSchedulerException
{
public:
    // SystemExecutionException();
    explicit SystemExecutionException(const std::string& msg)
        : TaskSchedulerException("System execution error: " + msg) {}

};

#endif // SYSTEMEXECUTIONEXCEPTION_H
