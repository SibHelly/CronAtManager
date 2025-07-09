#include "cronparser.h"


vector<CronTask> CronParser::parse_crontab_output(const string &crontab_output){
    vector<CronTask> tasks;
    istringstream stream(crontab_output);
    string line;

    while (getline(stream, line)) {
        // Пропускаем комментарии и пустые строки
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Парсим строку crontab: "* * * * * command"
        regex cron_regex(R"(^(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(.+)$)");
        smatch matches;

        if (regex_match(line, matches, cron_regex)) {
            CronTask task;
            task.cron_expression = matches[1].str() + " " + matches[2].str() + " " +
                                   matches[3].str() + " " + matches[4].str() + " " + matches[5].str();
            task.command = matches[6].str();
            task.original_crontab_line = line;
            tasks.push_back(task);
        }
    }

    return tasks;
}

vector<CronTask> CronParser::parse_crontab_logs(const string &output){
    vector<CronTask> tasks;
    istringstream stream(output);
    string line;

    while (getline(stream, line)){
        CronTask task;
        smatch matches;

        // Регулярные выражения для извлечения данных
        regex id_regex(R"(ID: (\d+) \|\|)");
        regex command_regex(R"(Command: ([^|]*) \|\|)");
        regex desc_regex(R"(Description: ([^|]*) \|\|)");
        regex created_regex(R"(Created at: (\d{2}:\d{2}:\d{2} \d{4}/\d{2}/\d{2}) \|\|)");
        regex cron_expr_regex(R"(Cron expression: ([^|]*) \|\|)");
        regex original_regex(R"(Original crontab line: ([^|]*) \|\|)");
        regex active_regex(R"(Is active: (true|false) \|\|)");

        // Парсинг ID
        if (regex_search(line, matches, id_regex) && matches.size() > 1) {
            task.id = stoi(matches[1].str());
        }

        // Парсинг Command
        if (regex_search(line, matches, command_regex) && matches.size() > 1) {
            task.command = matches[1].str();
        }

        // Парсинг Description
        if (regex_search(line, matches, desc_regex) && matches.size() > 1) {
            task.description = matches[1].str();
        }

        // Парсинг Created at
        if (regex_search(line, matches, created_regex) && matches.size() > 1) {
            string time_str = matches[1].str();
            tm tm = {};
            istringstream ss(time_str);
            ss >> get_time(&tm, "%H:%M:%S %Y/%m/%d");
            task.created_at = chrono::system_clock::from_time_t(mktime(&tm));
        }

        // Парсинг Cron expression
        if (regex_search(line, matches, cron_expr_regex) && matches.size() > 1) {
            task.cron_expression = matches[1].str();
        }

        // Парсинг Original crontab line
        if (regex_search(line, matches, original_regex) && matches.size() > 1) {
            task.original_crontab_line = matches[1].str();
        }

        // Парсинг Is active
        if (regex_search(line, matches, active_regex) && matches.size() > 1) {
            task.is_active = (matches[1].str() == "true");
        }

        tasks.push_back(task);
    }
    return tasks;

}

bool CronParser::is_valid_cron_expression(const string& expression){
    cout << expression << endl;
    regex cron_regex(R"(^(\*|\d+|\d+-\d+|\d+(,\d+)*|\*/\d+|\d+-\d+/\d+)(\s+(\*|\d+|\d+-\d+|\d+(,\d+)*|\*/\d+|\d+-\d+/\d+)){4}$)");

    return regex_match(expression, cron_regex);
}

string CronParser::create_crontab_line(const CronTask &task){
    string result = task.cron_expression + " " + task.command;
    return result;
}
