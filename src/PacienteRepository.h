#pragma once

#include "Database.h"
#include "Entidades.h"
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class PacienteRepository
{
private:
    Database &db;

    static Paciente montarPaciente(const pqxx::row &linha)
    {
        Paciente paciente;
        paciente.id = linha["id_paciente"].as<int>();
        paciente.cpf = linha["cpf"].c_str();
        paciente.nomeCompleto = linha["nome"].c_str();

        if (!linha["telefone"].is_null())
            paciente.telefone = linha["telefone"].c_str();
        if (!linha["endereco"].is_null())
            paciente.endereco = linha["endereco"].c_str();

        return paciente;
    }

public:
    explicit PacienteRepository(Database &database) : db(database) {}

    bool atualizar(const Paciente &paciente)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        const std::optional<std::string> telefone = paciente.telefone.empty()
                                                        ? std::nullopt
                                                        : std::optional<std::string>{paciente.telefone};
        const std::optional<std::string> endereco = paciente.endereco.empty()
                                                        ? std::nullopt
                                                        : std::optional<std::string>{paciente.endereco};

        const pqxx::result resultado = transacao.exec(
            "UPDATE Pacientes SET nome = $1, telefone = $2, endereco = $3 "
            "WHERE id_paciente = $4 RETURNING id_paciente",
            pqxx::params{paciente.nomeCompleto, telefone, endereco, paciente.id});

        transacao.commit();
        return !resultado.empty();
    }

    bool deletar(int pacienteId)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        const pqxx::result resultado = transacao.exec(
            "DELETE FROM Pacientes WHERE id_paciente = $1 RETURNING id_paciente",
            pqxx::params{pacienteId});

        transacao.commit();
        return !resultado.empty();
    }

    int cadastrar(const Paciente &paciente)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        const std::optional<std::string> telefone = paciente.telefone.empty()
                                                        ? std::nullopt
                                                        : std::optional<std::string>{paciente.telefone};
        const std::optional<std::string> endereco = paciente.endereco.empty()
                                                        ? std::nullopt
                                                        : std::optional<std::string>{paciente.endereco};

        const pqxx::result resultado = transacao.exec(
            "INSERT INTO Pacientes (cpf, nome, telefone, endereco) "
            "VALUES ($1, $2, $3, $4) RETURNING id_paciente",
            pqxx::params{paciente.cpf, paciente.nomeCompleto, telefone, endereco});

        const int id = resultado[0][0].as<int>();
        transacao.commit();
        return id;
    }

    std::optional<Paciente> buscarPorCPF(const std::string &cpf)
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "SELECT id_paciente, cpf, nome, telefone, endereco "
            "FROM Pacientes WHERE cpf = $1",
            pqxx::params{cpf});

        if (resultado.empty())
            return std::nullopt;

        return montarPaciente(resultado[0]);
    }

    std::vector<Paciente> buscarPorNome(const std::string &nomeParcial)
    {
        db.exigirConexao();
        pqxx::read_transaction transacao(*db.getConexao());
        const pqxx::result resultado = transacao.exec(
            "SELECT id_paciente, cpf, nome, telefone, endereco "
            "FROM Pacientes "
            "WHERE nome ILIKE '%' || $1 || '%' "
            "ORDER BY nome, cpf LIMIT 20",
            pqxx::params{nomeParcial});

        std::vector<Paciente> pacientes;
        pacientes.reserve(resultado.size());
        for (const auto &linha : resultado)
            pacientes.push_back(montarPaciente(linha));

        return pacientes;
    }
};
