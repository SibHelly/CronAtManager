#ifndef TASKSCHEDULEREXCEPTION_H
#define TASKSCHEDULEREXCEPTION_H

#include <iostream>
using namespace std;

class TaskSchedulerException : public std::exception {
private:
    string message;

public:
    explicit TaskSchedulerException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

#endif // TASKSCHEDULEREXCEPTION_H
