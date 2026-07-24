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

    bool VerificarCPF(string &cpf)
    {
        int soma = 0;
        int verificador = 10;
        int resto;
        for (int i = 0; i < cpf.length() - 2; i++)
        {
            soma += (int)cpf[i] * verificador;
            verificador--;
        }
        resto = soma % 11;
        if (resto == 0 || resto == 1)
        {
            if (cpf[9] != 0)
                return false;
        }
        else
        {
            resto = 11 - resto;
            if (cpf[9] != resto)
                return false;
        }
        verificador = 10;
        soma = 0;
        for (int i = 1; i < cpf.length() - 1; i++)
        {
            soma += cpf[i] * verificador;
            verificador--;
        }
        resto = soma % 11;
        if (resto == 0 || resto == 1)
        {
            if (cpf[10] != 0)
                return false;
        }
        else
        {
            resto = 11 - resto;
            if (cpf[10] != resto)
                return false;
        }
        return true;
    }

    // Retorna o ID gerado (>0) em caso de sucesso, ou std::nullopt em caso de erro.
    std::optional<int> cadastrar(const Paciente &paciente)
    {
        try
        {
            pqxx::work transacao(*db.getConexao());
            pqxx::result res = transacao.exec_params(
                "INSERT INTO Pacientes (cpf, nome, telefone, endereco) VALUES ($1, $2, $3, $4) "
                "RETURNING id_paciente",
                paciente.cpf, paciente.nomeCompleto, paciente.telefone, paciente.endereco);
            transacao.commit();
            int id = res[0][0].as<int>();
            std::cout << "Paciente cadastrado com sucesso! ID: " << id << std::endl;
            return id;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao cadastrar paciente: " << e.what() << std::endl;
            return std::nullopt;
        }
    }

    std::optional<Paciente> buscarPorCPF(const std::string &cpf)
    {
        try
        {
            pqxx::work transacao(*db.getConexao());
            pqxx::result res = transacao.exec_params("SELECT id_paciente, cpf, nome, telefone, endereco FROM Pacientes WHERE cpf = $1", cpf);

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