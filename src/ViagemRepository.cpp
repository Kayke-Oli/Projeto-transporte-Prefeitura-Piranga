#include "ViagemRepository.h"
#include <pqxx/pqxx>
#include <iostream>

ViagemRepository::ViagemRepository(Database &database) : db(database) {}

void ViagemRepository::cadastrarViagem(const Viagem &viagem)
{
    try
    {
        pqxx::work transacao(*db.getConexao());

        // Insere a viagem e retorna o ID gerado para ser usado na tabela N:N
        pqxx::result res = transacao.exec_params(
            "INSERT INTO Viagens (data_viagem, cidade_destino, id_carro, id_motorista) "
            "VALUES ($1, $2, $3, $4) RETURNING id_viagem",
            viagem.dataViagem, viagem.cidadeDestino, viagem.veiculoId, viagem.motoristaId);

        int id_viagem = res[0][0].as<int>();

        // Insere os pacientes e acompanhantes na tabela associativa
        for (const auto &pv : viagem.passageiros)
        {
            if (pv.acompanhanteId.has_value())
            {
                transacao.exec_params(
                    "INSERT INTO Viagem_Pacientes (id_viagem, id_paciente, id_acompanhante) VALUES ($1, $2, $3)",
                    id_viagem, pv.pacienteId, pv.acompanhanteId.value());
            }
            else
            {
                transacao.exec_params(
                    "INSERT INTO Viagem_Pacientes (id_viagem, id_paciente, id_acompanhante) VALUES ($1, $2, NULL)",
                    id_viagem, pv.pacienteId);
            }
        }

        transacao.commit();
        std::cout << "Viagem cadastrada com sucesso! ID da Viagem: " << id_viagem << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao cadastrar viagem: " << e.what() << std::endl;
    }
}

void ViagemRepository::gerarRelatorioHistoricoPaciente(const std::string &cpfPaciente)
{
    try
    {
        pqxx::work transacao(*db.getConexao());

        std::string sql =
            "SELECT v.data_viagem, v.cidade_destino, m.nome AS motorista, "
            "c.modelo AS veiculo_modelo, c.placa AS veiculo_placa, "
            "a.nome AS acompanhante "
            "FROM Viagem_Pacientes vp "
            "JOIN Pacientes p ON p.id_paciente = vp.id_paciente "
            "JOIN Viagens v ON v.id_viagem = vp.id_viagem "
            "JOIN Carros c ON c.id_carro = v.id_carro "
            "JOIN Motoristas m ON m.id_motorista = v.id_motorista "
            "LEFT JOIN Acompanhantes a ON a.id_acompanhante = vp.id_acompanhante "
            "WHERE p.cpf = $1 "
            "ORDER BY v.data_viagem DESC";

        pqxx::result res = transacao.exec_params(sql, cpfPaciente);

        std::cout << "\n--- Histórico de Viagens do Paciente (CPF: " << cpfPaciente << ") ---\n";
        for (auto row : res)
        {
            std::cout << "Data: " << row["data_viagem"].c_str()
                      << " | Destino: " << row["cidade_destino"].c_str()
                      << " | Motorista: " << row["motorista"].c_str()
                      << " | Carro: " << row["veiculo_modelo"].c_str() << " (" << row["veiculo_placa"].c_str() << ")";

            if (!row["acompanhante"].is_null())
            {
                std::cout << " | Acompanhante: " << row["acompanhante"].c_str();
            }
            else
            {
                std::cout << " | Acompanhante: Nenhum";
            }
            std::cout << "\n";
        }
        std::cout << "--------------------------------------------------\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao gerar histórico: " << e.what() << std::endl;
    }
}

int ViagemRepository::gerarRelatorioVolumePassageiros(const std::string &dataInicio, const std::string &dataFim)
{
    try
    {
        pqxx::work transacao(*db.getConexao());

        std::string sql =
            "SELECT "
            "(SELECT COUNT(*) FROM Viagem_Pacientes vp JOIN Viagens v ON vp.id_viagem = v.id_viagem WHERE v.data_viagem BETWEEN $1 AND $2) + "
            "(SELECT COUNT(id_acompanhante) FROM Viagem_Pacientes vp JOIN Viagens v ON vp.id_viagem = v.id_viagem WHERE v.data_viagem BETWEEN $1 AND $2) "
            "AS total_pessoas";

        pqxx::result res = transacao.exec_params(sql, dataInicio, dataFim);

        int total = res[0]["total_pessoas"].as<int>();
        std::cout << "Total de passageiros transportados entre " << dataInicio << " e " << dataFim << ": " << total << std::endl;
        return total;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao calcular volume de passageiros: " << e.what() << std::endl;
        return 0;
    }
}

void ViagemRepository::gerarMapaViagemDiario(const std::string &dataViagem)
{
    try
    {
        pqxx::work transacao(*db.getConexao());

        std::string sql =
            "SELECT v.id_viagem, c.placa, c.modelo, m.nome AS motorista, v.cidade_destino, "
            "p.nome AS paciente, a.nome AS acompanhante "
            "FROM Viagens v "
            "JOIN Carros c ON v.id_carro = c.id_carro "
            "JOIN Motoristas m ON v.id_motorista = m.id_motorista "
            "JOIN Viagem_Pacientes vp ON v.id_viagem = vp.id_viagem "
            "JOIN Pacientes p ON vp.id_paciente = p.id_paciente "
            "LEFT JOIN Acompanhantes a ON vp.id_acompanhante = a.id_acompanhante "
            "WHERE v.data_viagem = $1 "
            "ORDER BY v.id_viagem";

        pqxx::result res = transacao.exec_params(sql, dataViagem);

        std::cout << "\n--- Mapa de Viagens Diário (" << dataViagem << ") ---\n";
        int current_viagem = -1;

        for (auto row : res)
        {
            int id_viagem = row["id_viagem"].as<int>();

            if (id_viagem != current_viagem)
            {
                std::cout << "\n[Viagem ID: " << id_viagem << " | Destino: " << row["cidade_destino"].c_str() << "]\n";
                std::cout << "Veículo: " << row["modelo"].c_str() << " (" << row["placa"].c_str() << ") | Motorista: " << row["motorista"].c_str() << "\n";
                std::cout << "Passageiros:\n";
                current_viagem = id_viagem;
            }

            std::cout << "  - Paciente: " << row["paciente"].c_str();
            if (!row["acompanhante"].is_null())
            {
                std::cout << " (Acompanhante: " << row["acompanhante"].c_str() << ")";
            }
            std::cout << "\n";
        }
        std::cout << "--------------------------------------------------\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao gerar mapa de viagem diário: " << e.what() << std::endl;
    }
}