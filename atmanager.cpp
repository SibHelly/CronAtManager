#include "atmanager.h"

AtManager::AtManager(unique_ptr<ISystemExecutor> exec) : executor(move(exec)){
    if (!executor)
        throw TaskSchedulerException("SystemExecutor cannot be null");
    load_from_logs();
}

vector<AtTask> AtManager::get_all_tasks() {
    vector<AtTask> result;
    result.reserve(tasks.size());

    for (const auto& [id, task] : tasks) {
        result.push_back(task);
    }

    return result;
}

bool AtManager::add_task(const AtTask& task){
    AtTask newTask = task;

    if (newTask.id.empty())
        newTask.id=generate_task_id(newTask);

    if (tasks.find(newTask.id)!=tasks.end()){
        cerr << "Task with ID" << newTask.id << "already exists" << endl;
        return false;
    }

    string time_str = AtParser::format_at_time(newTask.scheduled_time);

    string at_command = "at -q " + task.queue + " <<< '" + task.command + "' " + time_str + " 2>&1";
    string output = executor->execute_command(at_command);

    if (executor->get_last_exit_code() != 0) {
        cerr << "Failed to schedule at job: " << output << endl;
        return false;
    }

    regex job_regex(R"(job\s+(\d+))");
    smatch matches;

    if (regex_search(output, matches, job_regex)) {
        newTask.at_job_id = matches[1].str();
    } else {
        cerr << "Could not extract job ID from at output: " << output << endl;
        return false;
    }
    newTask.is_active = true;
    tasks[newTask.id] = newTask;
    writeAtTaskToFile(newTask);
    return true;
}

bool AtManager::remove_task(const string &task_id){
    AtTask task=tasks[task_id];
    executor->execute_command("at -r " + task.at_job_id);
    tasks.erase(task_id);
    executor->execute_command("rm "+ filename);
    for (const auto& [id, task] : tasks){
        writeAtTaskToFile(task);
    }
    return true;
}

optional<AtTask> AtManager:: get_task_by_id(const string &task_id){
    return tasks[task_id];
}

bool AtManager::update_task(const AtTask& task){
    tasks[task.id]=task;
    executor->execute_command("rm "+ filename);
    for (const auto& [id, task] : tasks){
        writeAtTaskToFile(task);
    }
    return true;
}

void AtManager::load_from_atq() {
    try {
        string output = executor->execute_command("atq");

        if (executor->get_last_exit_code() != 0) {
            cerr << "Failed to get at queue" << endl;
            return;
        }

        auto parsed_tasks = AtParser::parse_atq_output(output, executor);
        bool need_rewrite = false;

        for (auto [id, task]:tasks){
            bool found = false;
            for (auto& parsed_task : parsed_tasks){
                if (task.at_job_id == parsed_task.at_job_id){
                    found = true;
                    break;
                }
            }
            if (!found){
                if (task.is_active){
                    tasks[task.id].is_active = false;
                    tasks[task.id].is_executed = true;
                    need_rewrite =true;
                }
            }
        }

        for (auto& parsed_task:parsed_tasks){
            bool found = false;
            for (auto [id, task]:tasks){
                if (task.at_job_id == parsed_task.at_job_id){
                    found = true;
                    break;
                }
            }
            if (!found){
                if (parsed_task.id.empty()) {
                    parsed_task.id = generate_task_id(parsed_task);
                }
                parsed_task.is_active = true;
                parsed_task.is_executed = false;
                parsed_task.created_at = chrono::system_clock::now();
                tasks[parsed_task.id] = parsed_task;
                writeAtTaskToFile(parsed_task);
            }
        }

        if (need_rewrite) {
            executor->execute_command("rm " + filename);
            for (const auto& [id, task] : tasks) {
                writeAtTaskToFile(task);
            }
        }
    } catch (const exception& e) {
        throw AtParseException("Failed to load at queue: " + string(e.what()));
    }
}

void AtManager::load_from_logs(){
    try{
        struct stat buffer;
        if (stat(filename.c_str(), &buffer) != 0) {
            // Файл не существует, создаём новый
            std::ofstream out(filename, std::ios::binary);
            if (!out) {
                cerr << "Failed to create at file: " << filename << std::endl;
                return;
            }
            out.close();
            return;
        }

        ifstream in(filename, ios::binary);
        if (!in.is_open()) {
            cerr << "Cannot open at logs file at" << endl;
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
        tasks.clear();
        auto parsed_tasks = AtParser::parse_at_logs(output);
        for (auto& task : parsed_tasks) {
            if (task.id.empty()) {
                task.id = generate_task_id(task);
            }
            tasks[task.id] = task;
        }
    }catch(exception& e){
        throw AtParseException("Failed to load at logs" + string(e.what()));
        return;
    }
}

string AtManager::generate_task_id(const AtTask &task){
    string base = task.command + to_string(chrono::duration_cast<chrono::seconds>(task.scheduled_time.time_since_epoch()).count());
    hash<string> hasher;
    size_t hash = hasher(base);
    return "at_" + to_string(hash);
}



void AtManager::sync_with_system(){
    try{
        load_from_atq();
    } catch (const exception& e){
        cerr << "Error syncing with at queue: " << e.what() << endl;
    }
}

void AtManager::writeAtTaskToFile(const AtTask& task){
    ofstream outFile(filename, ios::app);
    if (!outFile.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }
    string time = AtParser::format_at_time(task.created_at);
    string scheduled_time = AtParser::format_at_time(task.scheduled_time);

    outFile << "ID: " << task.id << " || "
            << "Command: " << task.command << " || "
            << "Queue: " << task.queue << " || "
            << "Description: " << task.description << " || "
            << "Created at: " << time << " || "
            << "Start time: " << scheduled_time << " || "
            << "Id from atq: " << task.at_job_id<< " || "
            << "Is active: " << (task.is_active ? "true" : "false") << " || "
            << "Is executed: " << (task.is_executed ? "true" : "false");
    outFile << "\n";
}

void AtManager::clear(){
    executor->execute_command("rm " + filename);
    for (auto [id, task]:tasks){
        executor->execute_command("at -r " + task.at_job_id);
    }
    tasks.clear();
}





