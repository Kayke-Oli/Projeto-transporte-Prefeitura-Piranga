#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <iostream>
#include <string>

class Database
{
private:
    std::unique_ptr<pqxx::connection> conn;
    std::string connection_string;

public:
    // Altere a string de conexão conforme os dados do servidor da prefeitura
    Database(const std::string &conn_str = "dbname=prefeitura_viagens user=postgres password=admin host=127.0.0.1 port=5432")
        : connection_string(conn_str) {}

    void conectar()
    {
        try
        {
            conn = std::make_unique<pqxx::connection>(connection_string);
            if (conn->is_open())
            {
                std::cout << "Banco de dados conectado com sucesso!" << std::endl;
            }
            else
            {
                std::cerr << "Falha ao conectar no banco de dados." << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro de conexão: " << e.what() << std::endl;
        }
    }

    void desconectar()
    {
        if (conn && conn->is_open())
        {
            conn->disconnect();
        }
    }

    pqxx::connection *getConexao()
    {
        return conn.get();
    }
};