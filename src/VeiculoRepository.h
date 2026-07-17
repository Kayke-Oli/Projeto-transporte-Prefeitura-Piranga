#pragma once
#include "Entidades.h"
#include "Database.h"
#include <pqxx/pqxx>
#include <iostream>

class VeiculoRepository
{
private:
    Database &db;

public:
    VeiculoRepository(Database &database) : db(database) {}

    void cadastrar(const Carro &carro)
    {
        try
        {
            pqxx::work transacao(*db.getConexao());
            transacao.exec_params(
                "INSERT INTO Carros (placa, modelo) VALUES ($1, $2)",
                carro.placa, carro.modelo);
            transacao.commit();
            std::cout << "Veículo cadastrado com sucesso!" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao cadastrar carro: " << e.what() << std::endl;
        }
    }
};