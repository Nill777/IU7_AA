#include <sstream>
#include <iostream>
#include "database.hpp"

#define PLACE_HOLDER 0

Database::Database(const std::string& connection_string) {
    conn = std::make_unique<pqxx::connection>(connection_string);
    create_table_if_not_exists(); // Создание таблицы при инициализации
}

void Database::create_table_if_not_exists() {
    pqxx::work txn(*conn);
    txn.exec0(R"(
        CREATE TABLE IF NOT EXISTS t.recipes (
            id SERIAL PRIMARY KEY,
            issue_id INT,
            url TEXT,
            title TEXT,
            ingredients TEXT,
            steps TEXT,
            image_url TEXT,
            task_lifetime BIGINT,
            wait_time_read_queue BIGINT,
            wait_time_extract_queue BIGINT,
            wait_time_write_queue BIGINT,
            processing_time_read BIGINT,
            processing_time_extract BIGINT,
            processing_time_write BIGINT
        )
    )");

    // Очищаем таблицу, если она существует
    txn.exec0("TRUNCATE TABLE t.recipes");

    txn.commit();
}

void Database::print_recipe(Task task, std::string ingredients_str, std::string steps_str) {
    std::cout << "---recipe---:" << std::endl;
    std::cout << "ID: " << task.id << std::endl;
    std::cout << "Issue ID: " << task.issue_id << std::endl;
    std::cout << "URL: " << task.url << std::endl;
    std::cout << "Title: " << task.title << std::endl;
    std::cout << "Ingredients: " << ingredients_str << std::endl;
    std::cout << "Steps: " << steps_str << std::endl;
    std::cout << "Image URL: " << task.image_url << std::endl;
}

void Database::insert_recipe(const Task& task) {
    pqxx::work txn(*conn);

    // Преобразование ingredients в строку
    std::ostringstream ingredients_stream;
    for (const auto& ingredient : task.ingredients) {
        if (ingredients_stream.tellp() != 0) {
            ingredients_stream << "; "; // Разделитель
        }
        ingredients_stream << txn.esc(ingredient);
    }
    std::string ingredients_str = ingredients_stream.str();

    // Преобразование steps в строку
    std::ostringstream steps_stream;
    for (const auto& step : task.steps) {
        if (steps_stream.tellp() != 0) {
            steps_stream << "; ";
        }
        steps_stream << txn.esc(step);
    }
    std::string steps_str = steps_stream.str();

    // print_recipe(task, ingredients_str, steps_str);

    // Выполнение SQL-запроса
    txn.exec0("INSERT INTO t.recipes (id, issue_id, url, title, ingredients, steps, image_url, wait_time_read_queue, wait_time_extract_queue, wait_time_write_queue, processing_time_read, processing_time_extract) VALUES (" +
        std::to_string(task.id) + ", " +
        std::to_string(task.issue_id) + ", '" +
        txn.esc(task.url) + "', '" +
        txn.esc(task.title) + "', '" +
        txn.esc(ingredients_str) + "', '" +
        txn.esc(steps_str) + "', '" +
        txn.esc(task.image_url) + "', " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.start_read_time - task.enqueue_read_time).count()) + ", " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.start_extraction_time - task.enqueue_extraction_time).count()) + ", " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.start_write_time - task.enqueue_write_time).count()) + ", " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.end_read_time - task.start_read_time).count()) + ", " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.end_extraction_time - task.start_extraction_time).count()) + ")");

    txn.commit();
}

void Database::update_recipe(const Task& task) {
    pqxx::work txn(*conn);
    txn.exec0("UPDATE t.recipes SET task_lifetime = " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.destruction_time - task.creation_time).count()) + ", " +
        "processing_time_write = " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(task.end_write_time - task.start_write_time).count()) +
        " WHERE id = " + std::to_string(task.id));

    txn.commit();
}
