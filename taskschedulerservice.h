#ifndef TASKSCHEDULERSERVICE_H
#define TASKSCHEDULERSERVICE_H

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <chrono>
#include <set>
#include <algorithm>

#include "cronmanager.h"
#include "atmanager.h"
#include "systemexecutor.h"

using namespace std;

struct AtTaskFilter {
    std::string queue_filter;
    bool show_only_pending;
    bool show_only_completed;

    // Конструктор для инициализации по умолчанию
    AtTaskFilter() : queue_filter(""), show_only_pending(false), show_only_completed(false) {}

    // Метод для сброса фильтров
    void reset() {
        queue_filter.clear();
        show_only_pending = false;
        show_only_completed = false;
    }
};

struct AtTaskSort {
    bool by_scheduled_time;
    bool by_queue;
    bool by_status;
    bool by_at_id;
    bool ascending;

    // Конструктор для инициализации по умолчанию
    AtTaskSort() : by_scheduled_time(false), by_queue(false), by_status(false),
        by_at_id(false), ascending(true) {}

    // Метод для сброса сортировки
    void reset() {
        by_scheduled_time = false;
        by_queue = false;
        by_status = false;
        by_at_id = false;
        ascending = true;
    }
};
class TaskSchedulerService
{
private:
    unique_ptr<CronManager> cron_manager;
    unique_ptr<AtManager> at_manager;

    string config_dir;
public:
    explicit TaskSchedulerService();
    ~TaskSchedulerService() = default;
    // Инициализация
    bool initialize();

    // Работа с cron задачами
    bool add_cron_task(const string& cron_expr, const string& command,
                       const string& description = "");
    bool remove_cron_task(const string& task_id);
    bool update_cron_task_status(const string& task_id);
    vector<CronTask> get_cron_tasks();
    vector<CronTask> get_cron_tasks_filtred(bool show_all = true, bool show_active = true);
    CronTask get_cron_task(const string& task_id);
    bool update_cron_task(CronTask& task);
    void cron_sync_with_system();
    void clear_cron();

    // Работа с at задачами
    bool add_at_task(const chrono::system_clock::time_point& time, const string& command,
                     const string& description = "", const string& queue = "a");
    bool remove_at_task(const string& task_id);
    bool update_at_task(AtTask& task);
    vector<AtTask> get_at_tasks();
    vector<AtTask> get_at_tasks_filtred(const AtTaskFilter& filter = {},
                                        const AtTaskSort& sort = {});
    AtTask get_at_task(const string& task_id);
    void at_sync_with_system();
    void sync_all_tasks();
    void clear_at();

};

#endif // TASKSCHEDULERSERVICE_H
