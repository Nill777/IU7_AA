#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <set>
#include <chrono>

class Task {
public:
    // по идее сделать геттеры и сеттеры и поля private
    int id;
    int issue_id;
    std::string url;
    std::string title;
    std::set<std::string> ingredients;
    std::set<std::string> steps;
    std::string image_url;

    std::string file_name;
    std::string page_content;

    std::chrono::high_resolution_clock::time_point creation_time;

    std::chrono::high_resolution_clock::time_point enqueue_read_time; // Время постановки в очередь на чтение
    std::chrono::high_resolution_clock::time_point start_read_time; // Время начала чтения
    std::chrono::high_resolution_clock::time_point end_read_time; // Время окончания чтения

    std::chrono::high_resolution_clock::time_point enqueue_extraction_time; // Время постановки в очередь на извлечение данных
    std::chrono::high_resolution_clock::time_point start_extraction_time; // Время начала извлечения данных
    std::chrono::high_resolution_clock::time_point end_extraction_time; // Время окончания извлечения данных

    std::chrono::high_resolution_clock::time_point enqueue_write_time; // Время постановки в очередь на запись в хранилище
    std::chrono::high_resolution_clock::time_point start_write_time; // Время начала записи в хранилище
    std::chrono::high_resolution_clock::time_point end_write_time; // Время окончания записи в хранилище

    std::chrono::high_resolution_clock::time_point destruction_time;

    void parse_html();
    // Пустой конструктор
    Task() 
        : id(0),
          issue_id(0),
          url(""),
          title(""),
          ingredients(),
          steps(),
          image_url(""),
          file_name(""),
          page_content(""),

          creation_time(),
          enqueue_read_time(), 
          start_read_time(), 
          end_read_time(), 
          enqueue_extraction_time(), 
          start_extraction_time(), 
          end_extraction_time(), 
          enqueue_write_time(), 
          start_write_time(), 
          end_write_time(), 
          destruction_time() 
    {
        // доп логика
    }

    // Конструктор
    Task(int id, int issue_id, std::string file_name)
        : id(id),
          issue_id(issue_id),
          url(""), 
          title(""), 
          ingredients(), 
          steps(), 
          image_url(""), 
          file_name(file_name),
          page_content(""),

          creation_time(std::chrono::high_resolution_clock::now()), // Устанавливаем текущее время
          enqueue_read_time(), 
          start_read_time(), 
          end_read_time(), 
          enqueue_extraction_time(), 
          start_extraction_time(), 
          end_extraction_time(), 
          enqueue_write_time(), 
          start_write_time(), 
          end_write_time(), 
          destruction_time() 
    {
        // доп логика
    }

    Task& operator=(const Task& other) {
        if (this != &other) {
            id = other.id;
            issue_id = other.issue_id;
            url = other.url;
            title = other.title;
            ingredients = other.ingredients;
            steps = other.steps;
            image_url = other.image_url;
            file_name = other.file_name;
            page_content = other.page_content;

            creation_time = other.creation_time;
            enqueue_read_time = other.enqueue_read_time;
            start_read_time = other.start_read_time;
            end_read_time = other.end_read_time;
            enqueue_extraction_time = other.enqueue_extraction_time;
            start_extraction_time = other.start_extraction_time;
            end_extraction_time = other.end_extraction_time;
            enqueue_write_time = other.enqueue_write_time;
            start_write_time = other.start_write_time;
            end_write_time = other.end_write_time;
            destruction_time = other.destruction_time;
        }
        return *this;
    }
};

#endif
