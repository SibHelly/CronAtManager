#ifndef ATPARSEEXCEPTION_H
#define ATPARSEEXCEPTION_H

#include "taskschedulerexception.h"

class AtParseException:public TaskSchedulerException
{
public:
    explicit AtParseException(const string& msg)
        : TaskSchedulerException("At parse error: " + msg) {}
};

#endif // ATPARSEEXCEPTION_H
