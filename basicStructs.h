#ifndef BASICSTRUCTS_H
#define BASICSTRUCTS_H

#include <iostream>
#include <set>
#include <chrono>

using namespace std;

struct Task {
    string id; //id задачи
    string command; //команда задачи
    string description; //описание задачи
    chrono::system_clock::time_point created_at; //время создания
    bool is_active = true; // активность задачи

    Task() = default;
    Task(const string& cmd, const string& desc = "")
        : command(cmd), description(desc), created_at(chrono::system_clock::now()) {}
};

struct CronTask : public Task {
    string cron_expression; //выражение cron * * * * *
    string original_crontab_line; // оригинальная строка из cron

    CronTask() = default;
    CronTask(const string& expr, const string& cmd, const string& desc = "")
        : Task(cmd, desc), cron_expression(expr) {}
};

struct AtTask : public Task {
    chrono::system_clock::time_point scheduled_time; // время запуска задачи
    string at_job_id; // id задачи из системы
    string queue; // очередь
    bool is_executed = false; // статус выполнения

    AtTask() = default;
    AtTask(const chrono::system_clock::time_point& time, const string& cmd, const string& desc = "", const string& qu ="a")
        : Task(cmd, desc), scheduled_time(time), queue(qu) {}
};

#endif // BASICSTRUCTS_H
