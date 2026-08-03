#pragma once

#include "Database.h"
#include "Entidades.h"
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class VeiculoRepository
{
private:
    Database &db;
    static Veiculo montar(const pqxx::row &linha)
    {
        return Veiculo{linha["id_carro"].as<int>(), linha["placa"].c_str(), linha["modelo"].c_str()};
    }

public:
    explicit VeiculoRepository(Database &database) : db(database) {}
    int cadastrar(const Veiculo &veiculo)
    {
        db.exigirConexao(); pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("INSERT INTO Carros (placa, modelo) VALUES ($1, $2) RETURNING id_carro", pqxx::params{veiculo.placa, veiculo.modelo});
        const int id = resultado[0][0].as<int>(); transacao.commit(); return id;
    }
    bool atualizar(const Veiculo &veiculo)
    {
        db.exigirConexao(); pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("UPDATE Carros SET modelo = $1 WHERE id_carro = $2 RETURNING id_carro", pqxx::params{veiculo.modelo, veiculo.id});
        transacao.commit(); return !resultado.empty();
    }
    bool deletar(int id)
    {
        db.exigirConexao(); pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("DELETE FROM Carros WHERE id_carro = $1 RETURNING id_carro", pqxx::params{id});
        transacao.commit(); return !resultado.empty();
    }
    std::optional<Veiculo> buscarPorPlaca(const std::string &placa)
    {
        db.exigirConexao(); pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("SELECT id_carro, placa, modelo FROM Carros WHERE placa = $1", pqxx::params{placa});
        return resultado.empty() ? std::nullopt : std::optional<Veiculo>{montar(resultado[0])};
    }
    std::vector<Veiculo> listarTodos()
    {
        db.exigirConexao(); pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("SELECT id_carro, placa, modelo FROM Carros ORDER BY placa");
        std::vector<Veiculo> itens; itens.reserve(resultado.size());
        for (const auto &linha : resultado) itens.push_back(montar(linha));
        return itens;
    }
};
