#ifndef CRONPARSER_H
#define CRONPARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <iomanip>

#include "basicStructs.h"
#include "utils.h"
using namespace std;

class CronParser
{
public:
    // CronParser();

    static vector<CronTask> parse_crontab_output(const string &crontab_output);
    static vector<CronTask> parse_crontab_logs(const string &output);
    static bool is_valid_cron_expression(const string& expression);
    static string create_crontab_line(const CronTask &Task);

};

#endif // CRONPARSER_H
