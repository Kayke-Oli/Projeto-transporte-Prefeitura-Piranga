#include "ViagemRepository.h"

void ViagemRepository::validarPassageirosUnicos(const std::vector<PassageiroDaViagem> &passageiros) const
{
    std::vector<int> pacientesVistos;
    std::vector<int> acompanhantesVistos;

    for (const auto &p : passageiros)
    {
        if (std::find(pacientesVistos.begin(), pacientesVistos.end(), p.pacienteId) != pacientesVistos.end())
        {
            throw RegraNegocioException("Paciente já cadastrado nesta viagem.");
        }
        pacientesVistos.push_back(p.pacienteId);

        if (p.acompanhanteId.has_value())
        {
            int accId = p.acompanhanteId.value();
            if (std::find(acompanhantesVistos.begin(), acompanhantesVistos.end(), accId) != acompanhantesVistos.end())
            {
                throw RegraNegocioException("Acompanhante já vinculado a outro paciente nesta viagem.");
            }
            acompanhantesVistos.push_back(accId);
        }
    }
}

int ViagemRepository::registrarViagem(const Viagem &viagem)
{
    validarPassageirosUnicos(viagem.passageiros); // 1ª barreira: lógica de negócio

    sqlite3 *db = db_.handle();
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    try
    {
        Statement insertViagem(db,
                               "INSERT INTO viagens (data_viagem, cidade_destino, veiculo_id, motorista_id) "
                               "VALUES (?1, ?2, ?3, ?4);");
        insertViagem.bind(1, viagem.dataViagem);
        insertViagem.bind(2, viagem.cidadeDestino);
        insertViagem.bind(3, viagem.veiculoId);
        insertViagem.bind(4, viagem.motoristaId);
        insertViagem.step();

        int viagemId = static_cast<int>(sqlite3_last_insert_rowid(db));

        for (const auto &passageiro : viagem.passageiros)
        {
            Statement insertPassageiro(db,
                                       "INSERT INTO viagem_passageiros (viagem_id, paciente_id, acompanhante_id) "
                                       "VALUES (?1, ?2, ?3);");
            insertPassageiro.bind(1, viagemId);
            insertPassageiro.bind(2, passageiro.pacienteId);
            insertPassageiro.bindOptional(3, passageiro.acompanhanteId);
            insertPassageiro.step(); // se violar UNIQUE(viagem_id, paciente_id), lança exceção -> catch abaixo
        }

        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        return viagemId;
    }
    catch (...)
    {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // 2ª barreira: constraint do banco + rollback
        throw;
    }
}