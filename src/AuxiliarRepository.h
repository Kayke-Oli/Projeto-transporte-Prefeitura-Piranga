#pragma once

#include "Database.h"
#include "Entidades.h"

#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class AuxiliarRepository
{
private:
    Database &db;

    static AuxiliarViagem montar(const pqxx::row &linha)
    {
        AuxiliarViagem auxiliar;
        auxiliar.id = linha["id_auxiliar"].as<int>();
        auxiliar.nome = linha["nome"].c_str();
        auxiliar.cpf = linha["cpf"].c_str();
        if (!linha["telefone"].is_null())
            auxiliar.telefone = linha["telefone"].c_str();
        return auxiliar;
    }

public:
    explicit AuxiliarRepository(Database &database) : db(database) {}

    int cadastrar(const AuxiliarViagem &auxiliar)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const std::optional<std::string> telefone = auxiliar.telefone.empty()
                                                         ? std::nullopt
                                                         : std::optional<std::string>{auxiliar.telefone};
        const pqxx::result resultado = transacao.exec(
            "INSERT INTO Auxiliares_Viagem (cpf, nome, telefone) VALUES ($1, $2, $3) RETURNING id_auxiliar",
            pqxx::params{auxiliar.cpf, auxiliar.nome, telefone});
        const int id = resultado[0][0].as<int>();
        transacao.commit();
        return id;
    }

    bool atualizar(const AuxiliarViagem &auxiliar)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const std::optional<std::string> telefone = auxiliar.telefone.empty()
                                                         ? std::nullopt
                                                         : std::optional<std::string>{auxiliar.telefone};
        const pqxx::result resultado = transacao.exec(
            "UPDATE Auxiliares_Viagem SET nome = $1, telefone = $2 WHERE id_auxiliar = $3 RETURNING id_auxiliar",
            pqxx::params{auxiliar.nome, telefone, auxiliar.id});
        transacao.commit();
        return !resultado.empty();
    }

    bool deletar(int id)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "DELETE FROM Auxiliares_Viagem WHERE id_auxiliar = $1 RETURNING id_auxiliar", pqxx::params{id});
        transacao.commit();
        return !resultado.empty();
    }

    std::optional<AuxiliarViagem> buscarPorCPF(const std::string &cpf)
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "SELECT id_auxiliar, cpf, nome, telefone FROM Auxiliares_Viagem WHERE cpf = $1", pqxx::params{cpf});
        return resultado.empty() ? std::nullopt : std::optional<AuxiliarViagem>{montar(resultado[0])};
    }

    std::vector<AuxiliarViagem> listarTodos()
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "SELECT id_auxiliar, cpf, nome, telefone FROM Auxiliares_Viagem ORDER BY nome, cpf");
        std::vector<AuxiliarViagem> itens;
        itens.reserve(resultado.size());
        for (const auto &linha : resultado)
            itens.push_back(montar(linha));
        return itens;
    }
};
