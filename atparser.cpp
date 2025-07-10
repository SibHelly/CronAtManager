#include "atparser.h"

vector<AtTask> AtParser::parse_atq_output(const string& atq_output, unique_ptr<ISystemExecutor>& executor){
    vector<AtTask> tasks;
    istringstream stream(atq_output);
    string line;

    while (getline(stream, line)) {
        // Парсим вывод atq: "1 Mon Dec 25 09:00:00 2023 a user"
        // regex at_regex(R"(^(\d+)\s+(.+?)\s+[a-z]\s+(\w+)$)");
        regex at_regex(R"(^(\d+)\s+([a-zA-Z]+\s+[a-zA-Z]+\s+\d+\s+\d{2}:\d{2}:\d{2}\s+\d{4})\s+([a-z])\s+\w+$)");

        smatch matches;

        if (regex_match(line, matches, at_regex)) {
            AtTask task;
            task.at_job_id = matches[1].str();
            TaskUtils::trim(task.at_job_id);

            string time_str = matches[2].str();
            task.scheduled_time = parse_at_time(time_str);

            task.queue = matches[3].str();

            try {
                string cmd = "at -c " + task.at_job_id + " 2>/dev/null | tail -1";
                string delimetr = executor->execute_command(cmd);
                TaskUtils::trim(delimetr);
                string output = executor->execute_command("at -c " + task.at_job_id);
                size_t delimiter_pos = output.find(delimetr);
                if (delimiter_pos != string::npos) {
                    size_t cmd_start = delimiter_pos + delimetr.length()+1; // Позиция после первого перевода строки
                    size_t cmd_end = output.find(delimetr, cmd_start); // Последний перевод строки перед закрывающим разделителем

                    string command = output.substr(cmd_start, cmd_end - cmd_start);
                    command.erase(command.find_last_not_of(" \n\r\t") + 1);
                    command.erase(0, command.find_first_not_of(" \n\r\t"));
                    task.command=command;
                }

                task.description = "At job #" + task.at_job_id;
                TaskUtils::trim(task.description);
                task.created_at = chrono::system_clock::now();
                task.is_active=true;
            }catch(...){
                task.command = "";
                task.description = "At job #" + task.at_job_id;
            }

            tasks.push_back(task);
        }
    }

    return tasks;
}

vector<AtTask> AtParser::parse_at_logs(const string &at_file_output) {
    vector<AtTask> tasks;
    istringstream stream(at_file_output);
    string line;

    while (getline(stream, line)) {
        AtTask task;
        smatch matches;

        // Улучшенные регулярные выражения
        regex id_regex(R"(ID:\s*([^|]*)\s*\|\|)");
        regex command_regex(R"(Command:\s*([^|]*)\s*\|\|)");
        regex queue_regex(R"(Queue:\s*([^|]*)\s*\|\|)");
        regex desc_regex(R"(Description:\s*([^|]*)\s*\|\|)");
        regex created_regex(R"(Created at:\s*(\d{2}:\d{2} \d{2}/\d{2}/\d{4})\s*\|\|)");
        regex start_regex(R"(Start time:\s*(\d{2}:\d{2} \d{2}/\d{2}/\d{4})\s*\|\|)");
        regex atq_id_regex(R"(Id from atq:\s*([^|]*)\s*\|\|)");
        regex active_regex(R"(Is active:\s*(true|false)\s*\|\|)");
        regex executed_regex(R"(Is executed:\s*(true|false)\s*(?:\|\|)?$)");

        // Парсинг всех полей
        if (regex_search(line, matches, id_regex) && matches.size() > 1) {
            task.id = matches[1].str();
            TaskUtils::trim(task.id);
        }

        if (regex_search(line, matches, command_regex) && matches.size() > 1) {
            task.command = matches[1].str();
            TaskUtils::trim(task.command);
        }

        if (regex_search(line, matches, queue_regex) && matches.size() > 1) {
            task.queue = matches[1].str();
            TaskUtils::trim(task.queue);
        }

        if (regex_search(line, matches, desc_regex) && matches.size() > 1) {
            task.description = matches[1].str();
            TaskUtils::trim(task.description);
        }

        if (regex_search(line, matches, created_regex) && matches.size() > 1) {
            string time_str = matches[1].str();
            TaskUtils::trim(time_str);
            task.created_at = parse_at_timeSlash(time_str);
        }

        if (regex_search(line, matches, start_regex) && matches.size() > 1) {
            string time_str = matches[1].str();
            TaskUtils::trim(time_str);
            task.scheduled_time = parse_at_timeSlash(time_str);
        }

        if (regex_search(line, matches, atq_id_regex) && matches.size() > 1) {
            task.at_job_id = matches[1].str();
            TaskUtils::trim(task.at_job_id);
        }

        if (regex_search(line, matches, active_regex) && matches.size() > 1) {
            string value = matches[1].str();
            TaskUtils::trim(value);
            task.is_active = (value == "true");

        }

        if (regex_search(line, matches, executed_regex) && matches.size() > 1) {
            string value = matches[1].str();
            TaskUtils::trim(value);
            task.is_executed = (value == "true");
        } else {
            cout << "Executed not found in line: " << line << endl;
        }

        tasks.push_back(task);
    }

    return tasks;
}

string AtParser::format_at_time(const chrono::system_clock::time_point& time){
    auto time_t = chrono::system_clock::to_time_t(time);
    stringstream ss;

    ss << put_time(localtime(&time_t),"%H:%M %m/%d/%Y");
    return ss.str();
}

// пример строкки времени возвращаемой atq - Sun Jun 29 13:00:00 2025
chrono::system_clock::time_point AtParser::parse_at_time(const string& time_str){
    tm tm = {};
    istringstream ss(time_str);

    if (ss >> get_time (&tm, "%a %b %d %H:%M:%S %Y")){
        return chrono::system_clock::from_time_t(mktime(&tm));
    }

    return chrono::system_clock::now();
}

chrono::system_clock::time_point AtParser::parse_at_timeSlash(const string& time_str) {
    tm tm = {};
    istringstream ss(time_str);

    // Парсим строку в соответствии с форматом "HH:%M %m/%d/%Y"
    ss >> get_time(&tm, "%H:%M %m/%d/%Y");

    if (ss.fail()) {
        throw runtime_error("Failed to parse time string");
    }

    // Преобразуем tm в time_t (учитываем локальное время)
    time_t tt = mktime(&tm);
    if (tt == -1) {
        throw runtime_error("Failed to convert tm to time_t");
    }

    // Преобразуем time_t в system_clock::time_point
    return chrono::system_clock::from_time_t(tt);
}

