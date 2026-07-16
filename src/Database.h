// Database.h
#pragma once
#include <sqlite3.h>
#include <memory>
#include <string>
#include <stdexcept>

class Database
{
public:
    explicit Database(const std::string &caminhoArquivo);
    ~Database() = default;

    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    sqlite3 *handle() const { return db_.get(); }
    void executarScript(const std::string &sql); // usado para rodar o CREATE TABLE inicial

private:
    struct SqliteDeleter
    {
        void operator()(sqlite3 *p) const { sqlite3_close_v2(p); }
    };
    std::unique_ptr<sqlite3, SqliteDeleter> db_;
};

// Wrapper de sqlite3_stmt* para garantir finalize() mesmo em caso de exceção
class Statement
{
public:
    Statement(sqlite3 *db, const std::string &sql);
    ~Statement();

    Statement(const Statement &) = delete;
    Statement &operator=(const Statement &) = delete;

    void bind(int index, int valor);
    void bind(int index, const std::string &valor);
    void bindNull(int index);
    void bindOptional(int index, const std::optional<std::string> &valor);
    void bindOptional(int index, const std::optional<int> &valor);

    bool step(); // true enquanto houver SQLITE_ROW
    int columnInt(int index) const;
    std::string columnText(int index) const;
    std::optional<std::string> columnTextOptional(int index) const;
    std::optional<int> columnIntOptional(int index) const;

private:
    sqlite3_stmt *stmt_ = nullptr;
};