#include "taskschedulerservice.h"

TaskSchedulerService::TaskSchedulerService(){
    auto executor = make_unique<SystemExecutor>();
    cron_manager = make_unique<CronManager>(make_unique<SystemExecutor>());
    at_manager = make_unique<AtManager>(make_unique<SystemExecutor>());
}

bool TaskSchedulerService::initialize() {
    try {
        sync_all_tasks();
        return true;
    } catch (const exception& e) {
        cerr << "Initialization failed: " << e.what() << endl;
        return false;
    }
}

void TaskSchedulerService::sync_all_tasks(){
    cron_manager->sync_with_system();
    at_manager->sync_with_system();
}

bool TaskSchedulerService::add_at_task(const chrono::system_clock::time_point& time, const string& command,
                                       const string& description, const string& queue){

    AtTask task(time, command, description, queue);

    if (at_manager->add_task(task)){
        return true;
    }

    return false;
}

vector<AtTask> TaskSchedulerService::get_at_tasks(){
    return at_manager->get_all_tasks();
}

vector<AtTask> TaskSchedulerService::get_at_tasks_filtred(const AtTaskFilter& filter,
                                                const AtTaskSort& sort_params)
{
    vector<AtTask> tasks = at_manager->get_all_tasks();
    vector<AtTask> result;

    // Фильтрация
    copy_if(tasks.begin(), tasks.end(), back_inserter(result),
        [&filter](const AtTask& task) {
            // Фильтр по очереди
            if (!filter.queue_filter.empty() && task.queue != filter.queue_filter) {
                return false;
            }

            // Фильтр по статусу
            if (filter.show_only_pending && task.is_executed) return false;
            if (filter.show_only_completed && !task.is_executed) return false;

            return true;
        });

    // Сортировка
    auto compareTasks = [&sort_params](const AtTask& a, const AtTask& b) {
        if (sort_params.by_status) {
            if (a.is_executed != b.is_executed) {
                return sort_params.ascending ? (a.is_executed < b.is_executed)
                                          : (a.is_executed > b.is_executed);
            }
        }

        if (sort_params.by_scheduled_time) {
            if (a.scheduled_time != b.scheduled_time) {
                return sort_params.ascending ? (a.scheduled_time < b.scheduled_time)
                                          : (a.scheduled_time > b.scheduled_time);
            }
        }

        if (sort_params.by_queue) {
            if (a.queue != b.queue) {
                return sort_params.ascending ? (a.queue < b.queue)
                                          : (a.queue > b.queue);
            }
        }

        if (sort_params.by_at_id) {
            return sort_params.ascending ? (a.at_job_id < b.at_job_id)
                                      : (a.at_job_id > b.at_job_id);
        }

        // По умолчанию сортируем по времени
        return sort_params.ascending ? (a.at_job_id < b.at_job_id)
                                     : (a.at_job_id > b.at_job_id);
    };

    std::sort(result.begin(), result.end(), compareTasks);

    return result;
}

bool TaskSchedulerService::remove_at_task(const string &task_id){
    return at_manager->remove_task(task_id);
}

bool TaskSchedulerService::update_at_task(AtTask &task){
    return at_manager->update_task(task);
}

AtTask TaskSchedulerService::get_at_task(const string& task_id){
    auto task = at_manager->get_task_by_id(task_id);
    if (!task) {
        throw std::runtime_error("At Task not found with id: " + task_id);
    }
    return *task;
}

bool TaskSchedulerService::add_cron_task(const string &cron_expr, const string &command,
                                         const string &description){
    if (!CronParser::is_valid_cron_expression(cron_expr)) {
        return false;
    }

    CronTask task(cron_expr, command, description);

    if (cron_manager->add_task(task)) {
        return true;
    }

    return false;
}

vector<CronTask> TaskSchedulerService::get_cron_tasks(){
    return cron_manager->get_all_tasks();
}

vector<CronTask> TaskSchedulerService::get_cron_tasks_filtred(bool show_all, bool show_active){
    auto all_tasks = cron_manager->get_all_tasks();
    if (show_all) {
        return all_tasks;
    }

    vector<CronTask> filtered_tasks;
    for (const auto& task : all_tasks) {
        if (show_active ? task.is_active : !task.is_active) {
            filtered_tasks.push_back(task);
        }
    }
    return filtered_tasks;
}

bool TaskSchedulerService::remove_cron_task(const string& task_id){
    return cron_manager->remove_task(task_id);
}

bool TaskSchedulerService::update_cron_task_status(const string& task_id){
    return cron_manager->toggle_task(task_id);
}

bool TaskSchedulerService::update_cron_task(CronTask& task){
    return cron_manager->update_task(task);
}

CronTask TaskSchedulerService::get_cron_task(const string& task_id){
    auto task = cron_manager->get_task_by_id(task_id);
    if (!task) {
        throw std::runtime_error("Cron Task not found with id: " + task_id);
    }
    return *task;
}

void TaskSchedulerService::cron_sync_with_system(){
    cron_manager->sync_with_system();
}

void TaskSchedulerService::at_sync_with_system(){
    at_manager->sync_with_system();
}

void TaskSchedulerService::clear_at(){
    at_manager->clear();
}

void TaskSchedulerService::clear_cron(){
    cron_manager->clear();
}
