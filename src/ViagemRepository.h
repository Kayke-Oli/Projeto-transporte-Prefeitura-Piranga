#pragma once
#include "Database.h"
#include "Entidades.h"
#include <vector>
#include <stdexcept>
#include <string>

class RegraNegocioException : public std::runtime_error
{
public:
    explicit RegraNegocioException(const std::string &msg) : std::runtime_error(msg) {}
};

class ViagemRepository
{
private:
    Database &db;

public:
    ViagemRepository(Database &database);

    void cadastrarViagem(const Viagem &viagem);
    void gerarRelatorioHistoricoPaciente(const std::string &cpfPaciente);
    int gerarRelatorioVolumePassageiros(const std::string &dataInicio, const std::string &dataFim);
    void gerarMapaViagemDiario(const std::string &dataViagem);
};

#endif