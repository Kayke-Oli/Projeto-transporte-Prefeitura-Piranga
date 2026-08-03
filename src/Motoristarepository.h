#pragma once

#include "Database.h"
#include "Entidades.h"
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class MotoristaRepository
{
private:
    Database &db;
    static Motorista montar(const pqxx::row &linha)
    {
        return Motorista{linha["id_motorista"].as<int>(), linha["nome"].c_str(), linha["cpf"].c_str()};
    }

public:
    explicit MotoristaRepository(Database &database) : db(database) {}
    int cadastrar(const Motorista &motorista)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("INSERT INTO Motoristas (nome, cpf) VALUES ($1, $2) RETURNING id_motorista", pqxx::params{motorista.nome, motorista.cpf});
        const int id = resultado[0][0].as<int>();
        transacao.commit();
        return id;
    }
    bool atualizar(const Motorista &motorista)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("UPDATE Motoristas SET nome = $1 WHERE id_motorista = $2 RETURNING id_motorista", pqxx::params{motorista.nome, motorista.id});
        transacao.commit();
        return !resultado.empty();
    }
    bool deletar(int id)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("DELETE FROM Motoristas WHERE id_motorista = $1 RETURNING id_motorista", pqxx::params{id});
        transacao.commit();
        return !resultado.empty();
    }
    std::optional<Motorista> buscarPorCPF(const std::string &cpf)
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("SELECT id_motorista, nome, cpf FROM Motoristas WHERE cpf = $1", pqxx::params{cpf});
        return resultado.empty() ? std::nullopt : std::optional<Motorista>{montar(resultado[0])};
    }
    std::vector<Motorista> listarTodos()
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec("SELECT id_motorista, nome, cpf FROM Motoristas ORDER BY nome, cpf");
        std::vector<Motorista> itens;
        itens.reserve(resultado.size());
        for (const auto &linha : resultado)
            itens.push_back(montar(linha));
        return itens;
    }
};
