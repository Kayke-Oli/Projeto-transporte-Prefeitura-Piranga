#pragma once
#include "Entidades.h"
#include "Database.h"
#include <pqxx/pqxx>
#include <optional>
#include <iostream>

class MotoristaRepository
{
private:
    Database &db;

public:
    MotoristaRepository(Database &database) : db(database) {}

    // Retorna o ID gerado (>0) em caso de sucesso, ou std::nullopt em caso de erro.
    std::optional<int> cadastrar(const Motorista &motorista)
    {
        try
        {
            db.exigirConexao();
            pqxx::work transacao(*db.getConexao());
            pqxx::result res = transacao.exec_params(
                "INSERT INTO Motoristas (nome, cpf) VALUES ($1, $2) RETURNING id_motorista",
                motorista.nome, motorista.cpf);
            transacao.commit();
            int id = res[0][0].as<int>();
            std::cout << "Motorista cadastrado com sucesso! ID: " << id << std::endl;
            return id;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao cadastrar motorista: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
};