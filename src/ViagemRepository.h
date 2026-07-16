// ViagemRepository.hpp
#pragma once
#include "Database.h"
#include "Entidades.h"
#include <vector>
#include <stdexcept>

class RegraNegocioException : public std::runtime_error
{
public:
    explicit RegraNegocioException(const std::string &msg) : std::runtime_error(msg) {}
};

class ViagemRepository
{
public:
    explicit ViagemRepository(Database &db) : db_(db) {}

    // Cria a viagem e insere todos os passageiros dentro de UMA transação.
    // Valida em C++ (antes de ir ao banco) que não há paciente duplicado
    // nem acompanhante repetido na mesma lista -- a UNIQUE constraint do
    // banco é a segunda linha de defesa, não a primeira.
    int registrarViagem(const Viagem &viagem);

    // Adiciona um único paciente (com acompanhante opcional) a uma viagem já existente.
    // Lança RegraNegocioException se o paciente já estiver nessa viagem.
    void adicionarPassageiro(int viagemId, int pacienteId,
                             std::optional<int> acompanhanteId = std::nullopt);

    std::optional<Viagem> buscarPorId(int viagemId);

    // Relatório 2
    VolumePassageirosResultado relatorioVolumePassageiros(const std::string &dataInicial,
                                                          const std::string &dataFinal);

    // Relatório 3
    std::vector<MapaViagemItem> mapaViagemDoDia(const std::string &data);

private:
    Database &db_;
    void validarPassageirosUnicos(const std::vector<PassageiroDaViagem> &passageiros) const;
};