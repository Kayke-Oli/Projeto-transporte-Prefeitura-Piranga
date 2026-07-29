#pragma once
#include "Entidades.h"
#include "Database.h"
#include <pqxx/pqxx>
#include <optional>
#include <iostream>

class VeiculoRepository
{
private:
    Database &db;

public:
    VeiculoRepository(Database &database) : db(database) {}

    // Retorna o ID gerado (>0) em caso de sucesso, ou std::nullopt em caso de erro.
    std::optional<int> cadastrar(const Veiculo &veiculo)
    {
        try
        {
            db.exigirConexao();
            pqxx::work transacao(*db.getConexao());
            pqxx::result res = transacao.exec(
                "INSERT INTO Carros (placa, modelo) VALUES ($1, $2) RETURNING id_carro",
                pqxx::params{veiculo.placa, veiculo.modelo});
            transacao.commit();
            int id = res[0][0].as<int>();
            std::cout << "Veículo cadastrado com sucesso! ID: " << id << std::endl;
            return id;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao cadastrar veículo: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
};