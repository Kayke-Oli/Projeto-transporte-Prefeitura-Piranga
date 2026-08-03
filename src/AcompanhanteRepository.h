#pragma once

#include "Database.h"
#include "Entidades.h"
#include <optional>
#include <pqxx/pqxx>
#include <string>

class AcompanhanteRepository
{
private:
    Database &db;

    static Acompanhante montar(const pqxx::row &linha)
    {
        Acompanhante acompanhante;
        acompanhante.id = linha["id_acompanhante"].as<int>();
        acompanhante.cpf = linha["cpf"].c_str();
        acompanhante.nomeCompleto = linha["nome"].c_str();
        if (!linha["telefone"].is_null())
            acompanhante.telefone = linha["telefone"].c_str();
        return acompanhante;
    }

public:
    explicit AcompanhanteRepository(Database &database) : db(database) {}

    int cadastrar(const Acompanhante &acompanhante)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const std::optional<std::string> telefone = acompanhante.telefone.empty() ? std::nullopt : std::optional<std::string>{acompanhante.telefone};
        const pqxx::result resultado = transacao.exec(
            "INSERT INTO Acompanhantes (cpf, nome, telefone) VALUES ($1, $2, $3) RETURNING id_acompanhante",
            pqxx::params{acompanhante.cpf, acompanhante.nomeCompleto, telefone});
        const int id = resultado[0][0].as<int>();
        transacao.commit();
        return id;
    }

    bool atualizar(const Acompanhante &acompanhante)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const std::optional<std::string> telefone = acompanhante.telefone.empty() ? std::nullopt : std::optional<std::string>{acompanhante.telefone};
        const pqxx::result resultado = transacao.exec(
            "UPDATE Acompanhantes SET nome = $1, telefone = $2 WHERE id_acompanhante = $3 RETURNING id_acompanhante",
            pqxx::params{acompanhante.nomeCompleto, telefone, acompanhante.id});
        transacao.commit();
        return !resultado.empty();
    }

    bool deletar(int id)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "DELETE FROM Acompanhantes WHERE id_acompanhante = $1 RETURNING id_acompanhante", pqxx::params{id});
        transacao.commit();
        return !resultado.empty();
    }

    std::optional<Acompanhante> buscarPorCPF(const std::string &cpf)
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "SELECT id_acompanhante, cpf, nome, telefone FROM Acompanhantes WHERE cpf = $1", pqxx::params{cpf});
        return resultado.empty() ? std::nullopt : std::optional<Acompanhante>{montar(resultado[0])};
    }
};
