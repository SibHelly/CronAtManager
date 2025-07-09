#ifndef TASKSERVICE_H
#define TASKSERVICE_H

class TaskService
{
public:
    TaskService();
    ~TaskService();

    void AddTask();
    void GetTask();
    void GetTasks();

private:
    void init();
    void create();

};

#endif // TASKSERVICE_H
