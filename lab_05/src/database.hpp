#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <pqxx/pqxx>
#include "task.hpp"

class Database {
public:
    Database(const std::string& connection_string);
    void insert_recipe(const Task& task);
    void update_recipe(const Task& task);
private:
    std::unique_ptr<pqxx::connection> conn;
    void create_table_if_not_exists();
    void print_recipe(Task task, std::string ingredients_str, std::string steps_str);
};

#endif
