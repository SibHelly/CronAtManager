#include "cronmanager.h"

CronManager::CronManager(unique_ptr<ISystemExecutor> exec) : executor(move(exec)){
    cout<<"Creating CronMnager"<<endl;
    if (!executor)
        throw TaskSchedulerException("SystemExecutor cannot be null");
    cout<<filename<<endl;
    load_from_logs();

}

vector<CronTask> CronManager::get_all_tasks() {
    vector<CronTask> result;
    result.reserve(tasks.size());

    for (const auto& [id, task] : tasks) {
        result.push_back(task);
    }

    return result;
}

bool CronManager::add_task(const CronTask& task){
    try{

        if (!CronParser::is_valid_cron_expression(task.cron_expression)){
            cerr<<"Invalid cron expression: " << task.cron_expression << endl;
            return false;
        }

        CronTask newTask = task;
        if (newTask.id.empty()){
            newTask.id = generate_task_id(newTask);
        }

        if (tasks.find(newTask.id)!=tasks.end()){
            cerr << "Task with ID" << newTask.id << "already exists" << endl;
            return false;
        }
        newTask.created_at = chrono::system_clock::now();
        tasks[newTask.id] = newTask;

        if (!apply_changes_to_system()){
            tasks.erase(newTask.id);
            return false;
        }
        writeCronTaskToFile(newTask);
        cout<< "Cron task adding sucsessfuly: " << newTask.id << endl;
        return true;
    }catch ( const exception& e){
        cerr << "Error adding cron task: " << e.what() << endl;
        return false;
    }
}

bool CronManager::update_task(const CronTask& task){
    if (!CronParser::is_valid_cron_expression(task.cron_expression)){
        cerr<<"Invalid cron expression: " << task.cron_expression << endl;
        return false;
    }
    tasks[task.id]=task;
    apply_changes_to_system();
    return true;

}

bool CronManager::remove_task(const string &task_id){
    tasks.erase(task_id);
    apply_changes_to_system();
    return true;
}

bool CronManager::toggle_task(const string &task_id){
    tasks[task_id].is_active = !tasks[task_id].is_active ;
    apply_changes_to_system();
    return true;
}

optional<CronTask> CronManager:: get_task_by_id(const string& task_id){
    return tasks[task_id];
}

void CronManager::load_from_logs(){
    try{
        ifstream in(filename, ios::binary);
        if (!in.is_open()) {
            cerr << "Cannot open crontab logs file cron" << endl;
            return;
        }

        // Перемещаем указатель в конец файла, чтобы узнать его размер
        in.seekg(0, ios::end);
        size_t file_size = in.tellg();
        in.seekg(0, ios::beg);

        // Резервируем память и читаем весь файл
        string output(file_size, '\0');
        in.read(&output[0], file_size);
        in.close();

        auto parsed_tasks = CronParser::parse_crontab_logs(output);
        tasks.clear();
        for (auto& task : parsed_tasks) {
            if (task.id.empty()) {
                task.id = generate_task_id(task);
            }
            tasks[task.id] = task;
        }
    }catch(exception& e){
        throw CronParseException("Failed to load crontab logs" + string(e.what()));
        return;
    }
}

void CronManager::load_from_crontab(){
    try{
        string output = executor->execute_command("crontab -l 2>/dev/null");

        if (executor->get_last_exit_code()!=0){
            cout << "Not found cron tasks" << endl;
            return;
        }
        for (auto& existing_task : tasks) {
            existing_task.second.is_active = false;
        }

        auto parsed_tasks = CronParser::parse_crontab_output(output);
        for (auto& task : parsed_tasks) {
            if (task.id.empty()) {
                task.id = generate_task_id(task);
            }

            // Если задача уже существует, обновляем её и помечаем как активную
            if (tasks.find(task.id) != tasks.end()) {
                tasks[task.id].is_active = true; // Обновляем данные задачи
            }
            else {
                // Новая задача по умолчанию активна
                task.created_at = chrono::system_clock::now();
                task.is_active = true;
                tasks[task.id] = task;
                writeCronTaskToFile(task);
            }
        }


        cout << "Loaded " << tasks.size() << " tasks from crontab" << endl;


    }catch(exception& e){
        throw CronParseException("Failed to load crontab" + string(e.what()));
        return;
    }
}

void CronManager::updateLogs(){
    executor->execute_command("rm "+ filename);
    for (const auto& [id, task] : tasks){
        writeCronTaskToFile(task);
    }
}

bool CronManager::apply_changes_to_system(){
    try {
        // Создаем временный файл с новым crontab
        string temp_file = "/tmp/crontab_" + to_string(getpid());
        ofstream file(temp_file);

        if (!file.is_open()) {
            cerr << "Cannot create temporary crontab file: " << temp_file << endl;
            return false;
        }


        for (const auto& [id, task] : tasks) {
            if (task.is_active) {
                string cron_line = CronParser::create_crontab_line(task);
                file << cron_line << endl;
            }
        }

        file.close();

        // Устанавливаем новый crontab
        string command = "crontab " + temp_file;
        string output = executor->execute_command(command);

        // Удаляем временный файл
        remove(temp_file.c_str());

        if (executor->get_last_exit_code() != 0) {
            cerr << "Failed to install crontab: " << output << endl;
            return false;
        }

        updateLogs();

        return true;

    } catch (const exception& e) {
        cerr << "Error applying crontab changes: " << e.what() << endl;
        return false;
    }
}

string CronManager::generate_task_id(const CronTask& task){
    string base = task.command + task.cron_expression;
    hash<string> hasher;
    size_t hash = hasher(base);
    return "cron_" + to_string(hash);

}

void CronManager::sync_with_system(){
    try{
        load_from_crontab();
        cout <<  "Synchronized with system cron queue" << endl;
    } catch (const exception& e){
        cerr << "Error syncing with at queue: " << e.what() << endl;
    }
}

void CronManager::writeCronTaskToFile(const CronTask& task) {
    ofstream outFile(filename, ios::app);
    string dir = "/home/sibhelly/task_scheduler/";
    if (mkdir(dir.c_str(), 0755) == -1 && errno != EEXIST) {
        cerr << "Failed to create directory: " << dir << endl;
        return;
    }

    if (!outFile.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    // Конвертируем время в читаемый формат

    auto created_time = chrono::system_clock::to_time_t(task.created_at);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S %Y/%m/%d", localtime(&created_time));

    // Записываем все данные в одну строку с разделителями ||
    outFile << "ID: " << task.id << " || "
            << "Command: " << task.command << " || "
            << "Description: " << task.description << " || "
            << "Created at: " << time_str << " || "
            << "Cron expression: " << task.cron_expression << " || "
            << "Original crontab line: " << task.original_crontab_line << " || "
            << "Is active: " << (task.is_active ? "true" : "false");
    outFile << "\n";

    outFile.close();
}

void CronManager::clear(){
    executor->execute_command("rm " + filename);
    tasks.clear();
}


