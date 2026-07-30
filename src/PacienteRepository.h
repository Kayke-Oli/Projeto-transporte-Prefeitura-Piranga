#pragma once
#include "Entidades.h"
#include "Database.h"
#include <pqxx/pqxx>
#include <vector>
#include <optional>
#include <iostream>

class PacienteRepository
{
private:
    Database &db;

public:
    PacienteRepository(Database &database) : db(database) {}

    void atualizar(const Paciente &paciente)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        // Atualiza nome, telefone e endereço buscando pelo CPF
        transacao.exec(
            "UPDATE Pacientes SET nome = $1, telefone = $2, endereco = $3 WHERE cpf = $4",
            pqxx::params{paciente.nomeCompleto, paciente.telefone, paciente.endereco, paciente.cpf});

        transacao.commit();
    }

    void deletar(const std::string &cpf)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        // O banco vai disparar erro 23503 aqui se o paciente tiver viagens!
        transacao.exec(
            "DELETE FROM Pacientes WHERE cpf = $1",
            pqxx::params{cpf});

        transacao.commit();
    }

    int cadastrar(const Paciente &paciente)
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        // O pqxx vai lançar uma exceção automaticamente se violar restrições (como CPF duplicado)
        pqxx::result res = transacao.exec(
            "INSERT INTO Pacientes (cpf, nome, telefone, endereco) VALUES ($1, $2, $3, $4) RETURNING id_paciente",
            pqxx::params{paciente.cpf, paciente.nomeCompleto, paciente.telefone, paciente.endereco});

        transacao.commit();

        int id = res[0][0].as<int>();
        return id; // Retorna direto o ID gerado, ou lança exceção em caso de falha
    }

    std::optional<Paciente>
    buscarPorCPF(const std::string &cpf)
    {
        try
        {
            db.exigirConexao();
            pqxx::work transacao(*db.getConexao());
            pqxx::result res = transacao.exec("SELECT id_paciente, cpf, nome, telefone, endereco FROM Pacientes WHERE cpf = $1", pqxx::params{cpf});

            if (!res.empty())
            {
                Paciente p;
                p.id = res[0]["id_paciente"].as<int>();
                p.cpf = res[0]["cpf"].c_str();
                p.nomeCompleto = res[0]["nome"].c_str();
                p.telefone = res[0]["telefone"].c_str();
                p.endereco = res[0]["endereco"].c_str();
                return p;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao buscar paciente: " << e.what() << std::endl;
        }
        return std::nullopt;
    }
};