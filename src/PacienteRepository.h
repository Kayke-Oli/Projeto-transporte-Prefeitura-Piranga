// PacienteRepository.h
#pragma once
#include "Database.h"
#include "Entidades.h"
#include <vector>

class PacienteRepository
{
public:
    explicit PacienteRepository(Database &db) : db_(db) {}

    int cadastrar(const Paciente &paciente); // lança exceção se CPF duplicado
    std::optional<Paciente> buscarPorCpf(const std::string &cpf);
    std::optional<Paciente> buscarPorId(int id);
    std::vector<HistoricoPacienteItem> buscarHistorico(const std::string &cpf); // Relatório 1

private:
    Database &db_;
};