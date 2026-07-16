// VeiculoRepository.h / MotoristaRepository.h / AcompanhanteRepository.h
// (seguem o mesmo padrão CRUD simples)
#pragma once
#include "Database.h"
#include "Entidades.h"
#include <vector>
#include <stdexcept>

class VeiculoRepository
{
public:
    explicit VeiculoRepository(Database &db) : db_(db) {}
    int cadastrar(const Veiculo &v); // valida placa única
    std::optional<Veiculo> buscarPorId(int id);
    std::vector<Veiculo> listarTodos();

private:
    Database &db_;
};

class AcompanhanteRepository
{
public:
    explicit AcompanhanteRepository(Database &db) : db_(db) {}
    int cadastrar(const Acompanhante &a); // valida CPF único
    std::optional<Acompanhante> buscarPorCpf(const std::string &cpf);

private:
    Database &db_;
};