#pragma once
#include "Entidades.h"
#include "Database.h"
#include <pqxx/pqxx>
#include <iostream>

class AcompanhanteRepository
{
private:
    Database &db;

public:
    AcompanhanteRepository(Database &database) : db(database) {}

    void cadastrar(const Acompanhante &acompanhante)
    {
        try
        {
            pqxx::work transacao(*db.getConexao());
            transacao.exec_params(
                "INSERT INTO Acompanhantes (cpf, nome, telefone) VALUES ($1, $2, $3)",
                acompanhante.cpf, acompanhante.nome, acompanhante.telefone);
            transacao.commit();
            std::cout << "Acompanhante cadastrado com sucesso!" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao cadastrar acompanhante: " << e.what() << std::endl;
        }
    }
};

#endif