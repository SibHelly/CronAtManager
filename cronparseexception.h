#ifndef CRONPARSEEXCEPTION_H
#define CRONPARSEEXCEPTION_H
#include "taskschedulerexception.h"

class CronParseException: public TaskSchedulerException
{
public:
    CronParseException(const string& msg):
        TaskSchedulerException("Cron parse error: " + msg){}
};

#endif // CRONPARSEEXCEPTION_H
