#pragma once

#include "Database.h"
#include "Entidades.h"
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

class RegraNegocioException : public std::runtime_error
{
public:
    explicit RegraNegocioException(const std::string &mensagem) : std::runtime_error(mensagem) {}
};

class ViagemRepository
{
private:
    Database &db;

public:
    explicit ViagemRepository(Database &database);

    int cadastrarViagem(const Viagem &viagem);
    bool atualizarViagem(const Viagem &viagem);
    bool deletarViagem(int idViagem);
    std::optional<ViagemDetalhe> buscarPorId(int idViagem);
    std::vector<ViagemResumo> listarPorData(const std::string &dataViagem);
    std::optional<RelatorioViagem> gerarRelatorioViagem(int idViagem);

    std::vector<HistoricoPacienteItem> gerarRelatorioHistoricoPaciente(const std::string &cpfPaciente);
    VolumePassageirosResultado gerarRelatorioVolumePassageiros(const std::string &dataInicio, const std::string &dataFim);
    std::vector<MapaViagemItem> gerarMapaViagemDiario(const std::string &dataViagem);
};
