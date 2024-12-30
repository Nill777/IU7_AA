#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <random>
#include <condition_variable>
#include <filesystem>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include "task.hpp"
#include "database.hpp"

#define MAX_TASKS 3
#define ISSUE_ID 9194 //readmine
#define DATA "./data"

// Глобальные очереди
std::queue<Task> read_queue;
std::queue<Task> extract_queue;
std::queue<Task> write_queue;

// Мьютексы для каждой очереди
std::mutex read_queue_mutex;
std::mutex extract_queue_mutex;
std::mutex write_queue_mutex;

// Отслеживание завершенных задач
std::atomic<int> total_tasks(0);
std::atomic<int> read_total_tasks(0);
std::atomic<int> extract_total_tasks(0);
std::atomic<int> write_total_tasks(0);

std::condition_variable cv;


void get_num_tasks(std::atomic<int>& tasks) {
    int input_tasks = 0;
    while (true) {
        std::cout << "Enter the number of tasks to process: ";
        std::cin >> input_tasks; 
        if (input_tasks > 0 && input_tasks < MAX_TASKS) {
            tasks.store(input_tasks);
            break;
        }
        std::cout << "Invalid the number of tasks (0-" << MAX_TASKS << ")" << std::endl;
    }
}

std::vector<std::string> get_file_names(const std::string& directory, int count) {
    std::vector<std::string> file_names;
    int files_added = 0;

    std::filesystem::directory_iterator dir_iter(directory);
    std::filesystem::directory_iterator end_iter;

    while (dir_iter != end_iter) {
        if (std::filesystem::is_regular_file(*dir_iter)) {
            file_names.push_back(dir_iter->path().filename().string());
            files_added++;
            if (files_added >= count)
                break;
        }
        ++dir_iter;
    }
    return file_names;
}

void convert_file_encoding(const std::string& input_file, const std::string& output_file) {
    std::string command = "iconv -f WINDOWS-1251 -t UTF-8 " + input_file + " -o " + output_file;

    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Error: invalid iconv");
    }
}

void task_generator(const std::vector<std::string>& file_names) {
    int id = 1;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0.01, 0.05);

    for (const std::string& file_name : file_names) {
        std::this_thread::sleep_for(std::chrono::seconds(distribution(generator)));

        Task task(id, ISSUE_ID, file_name);
        std::cout << "Task: " << task.id << " created\nFile: " << file_name << std::endl;
        {
            std::lock_guard<std::mutex> lock(read_queue_mutex);
            task.enqueue_read_time = std::chrono::high_resolution_clock::now();
            read_queue.push(task);
            std::cout << "Task: " << task.id << " added to read queue" << std::endl;
        }
        ++id;
        cv.notify_one(); // Уведомление потоков
    }
}

void read_data() {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 5);

    while (true) {
        std::string src, dst;
        Task task;
        {
            std::unique_lock<std::mutex> lock(read_queue_mutex);
            cv.wait(lock, [] { return !read_queue.empty() || read_total_tasks.load() == 0; });
            
            // std::this_thread::sleep_for(std::chrono::seconds(distribution(generator)));
            if (read_total_tasks.load() == 0) {
                break; // Завершаем поток
            }

            task = read_queue.front();
            read_queue.pop();
        }

        // Обработка задачи
        task.start_read_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " start reading" << std::endl;

        // если файл ещё не в кодировке UTF-8 то конвертнуть
        src = std::string(DATA) + "/" + task.file_name;
        // std::string::npos максимум size_t и индекс "не найдено"
        if (task.file_name.find("utf_8") == std::string::npos) {
            dst = std::string(DATA) + "/utf_8_" + task.file_name;

            convert_file_encoding(src, dst);
            // Удаляем исходный файл
            std::remove(src.c_str());
            src = dst;
        }

        std::ifstream file(src);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        task.page_content = buffer.str();

        task.end_read_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " end reading" << std::endl;

        // После обработки, добавляем задачу в extract_queue
        {
            std::lock_guard<std::mutex> lock(extract_queue_mutex);
            task.enqueue_extraction_time = std::chrono::high_resolution_clock::now();
            extract_queue.push(task);
            std::cout << "Task: " << task.id << " added to extract queue" << std::endl;
        }

        --read_total_tasks;
        cv.notify_one();
    }
}

void extract_data() {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 5);

    while (true) {

        Task task;
        {
            std::unique_lock<std::mutex> lock(extract_queue_mutex);
            cv.wait(lock, [] { return !extract_queue.empty() || extract_total_tasks.load() == 0; });

            // std::this_thread::sleep_for(std::chrono::seconds(distribution(generator)));
            if (extract_total_tasks.load() == 0) {
                break; // Завершаем поток
            }

            task = extract_queue.front();
            extract_queue.pop();
        }

        // Обработка задачи
        task.start_extraction_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " start extracting" << std::endl;

        task.parse_html();

        task.end_extraction_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " end extracting" << std::endl;

        // После обработки, добавляем задачу в write_queue
        {
            std::lock_guard<std::mutex> lock(write_queue_mutex);
            task.enqueue_write_time = std::chrono::high_resolution_clock::now();
            write_queue.push(task);
            std::cout << "Task: " << task.id << " added to write queue" << std::endl;
        }

        --extract_total_tasks;
        cv.notify_one();
    }
}

void write_data(Database& db) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 5);

    while (true) {

        Task task;
        {
            std::unique_lock<std::mutex> lock(write_queue_mutex);
            cv.wait(lock, [] { return !write_queue.empty() || write_total_tasks.load() == 0; });

            // std::this_thread::sleep_for(std::chrono::seconds(distribution(generator)));
            if (write_total_tasks.load() == 0) {
                break; // Завершаем поток
            }

            task = write_queue.front();
            write_queue.pop();
        }

        // Обработка задачи
        task.start_write_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " start writing" << std::endl;

        db.insert_recipe(task);

        task.end_write_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " end writing" << std::endl;

        task.destruction_time = std::chrono::high_resolution_clock::now();
        std::cout << "Task: " << task.id << " destructed" << std::endl;

        db.update_recipe(task);

        --write_total_tasks;
        // cv.notify_one();
    }
}

int main() {
    Database db("postgresql://postgres:postgres@localhost:5432/postgres");
    get_num_tasks(total_tasks);
    read_total_tasks.store(total_tasks);
    extract_total_tasks.store(total_tasks);
    write_total_tasks.store(total_tasks);
    
    std::vector<std::string> file_names = get_file_names(DATA, total_tasks);

    // Запуск потоков
    std::thread generator(task_generator, file_names);
    std::thread reader(read_data);
    std::thread extractor(extract_data);
    std::thread writer(write_data, std::ref(db));

    // Ожидание завершения потоков
    generator.join();
    reader.join();
    extractor.join();
    writer.join();

    return 0;
}
