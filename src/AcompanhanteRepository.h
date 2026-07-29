#pragma once
#include "Entidades.h"
#include "Database.h"
#include <pqxx/pqxx>
#include <optional>
#include <iostream>

class AcompanhanteRepository
{
private:
    Database &db;

public:
    AcompanhanteRepository(Database &database) : db(database) {}

    // Retorna o ID gerado (>0) em caso de sucesso, ou std::nullopt em caso de erro.
    std::optional<int> cadastrar(const Acompanhante &acompanhante)
    {
        try
        {
            db.exigirConexao();
            pqxx::work transacao(*db.getConexao());
            pqxx::result res = transacao.exec(
                "INSERT INTO Acompanhantes (cpf, nome, telefone) VALUES ($1, $2, $3) "
                "RETURNING id_acompanhante",
                pqxx::params{acompanhante.cpf, acompanhante.nomeCompleto, acompanhante.telefone});
            transacao.commit();
            int id = res[0][0].as<int>();
            std::cout << "Acompanhante cadastrado com sucesso! ID: " << id << std::endl;
            return id;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao cadastrar acompanhante: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
};