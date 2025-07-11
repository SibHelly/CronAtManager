#ifndef CRONMANAGER_H
#define CRONMANAGER_H

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <chrono>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sys/stat.h>  // для mkdir

#include "isystemexecutor.h"
#include "basicStructs.h"
#include "cronparser.h"
#include "utils.h"
#include "taskschedulerexception.h"
#include "cronparseexception.h"

#include "conf.h"

using namespace std;

class CronManager
{
private:
    unique_ptr<ISystemExecutor> executor;
    map<string, CronTask> tasks;
public:
    explicit CronManager(unique_ptr<ISystemExecutor> exec);

    // Основные операции
    vector<CronTask> get_all_tasks();
    bool add_task(const CronTask& task);
    bool remove_task(const string& task_id);
    bool toggle_task(const string& task_id);
    bool update_task(const CronTask& updated_task);

    // Поиск и фильтрация
    optional<CronTask> get_task_by_id(const string& task_id);

    // Синхронизация с системой
    void sync_with_system();
    bool apply_changes_to_system();
    void updateLogs();
    void clear();

private:

    void load_from_logs();
    void load_from_crontab();
    string generate_task_id(const CronTask& task);
    void writeCronTaskToFile(const CronTask& task);
    void create_or_check_file();

    const string filename = getConfigPath() + "cronTasks.log";
};

#endif // CRONMANAGER_H
