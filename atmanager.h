#ifndef ATMANAGER_H
#define ATMANAGER_H

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>  // для mkdir
#include <set>
#include <unordered_set>

#include "isystemexecutor.h"
#include "basicStructs.h"
#include "atparser.h"
#include "utils.h"
#include "taskschedulerexception.h"
#include "atparseexception.h"
#include "globals.h"

using namespace std;

class AtManager
{
private:
    unique_ptr<ISystemExecutor> executor;
    map<string, AtTask> tasks;

public:
    explicit AtManager(unique_ptr<ISystemExecutor> exec);

    // Основные операции
    vector<AtTask> get_all_tasks();
    bool add_task(const AtTask& task);
    bool remove_task(const string& task_id);
    bool update_task(const AtTask& task);

    optional<AtTask> get_task_by_id(const string& task_id);

    // Синхронизация с системой
    void sync_with_system();
    void clear();
private:
    void load_from_atq();
    void load_from_logs();
    string generate_task_id(const AtTask& task);
    void writeAtTaskToFile(const AtTask& task);


    string filename = getConfigPath() + "atTasks.log";
};

#endif // ATMANAGER_H
