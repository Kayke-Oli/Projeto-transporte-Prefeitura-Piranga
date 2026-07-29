#include "ViagemRepository.h"
#include "DataUtils.h"
#include <pqxx/pqxx>
#include <iostream>

ViagemRepository::ViagemRepository(Database &database) : db(database) {}

void ViagemRepository::cadastrarViagem(const Viagem &viagem)
{
    try
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        // A coluna data_viagem agora é DATE; convertemos de "DD-MM-YYYY"
        // (formato do domínio, ver Entidades.h) para "YYYY-MM-DD" (ISO,
        // exigido pelo PostgreSQL) na borda com o banco.
        std::string dataViagemISO = DataUtils::paraISO(viagem.dataViagem);

        // Insere a viagem e retorna o ID gerado para ser usado na tabela N:N
        pqxx::result res = transacao.exec(
            "INSERT INTO Viagens (data_viagem, cidade_destino, id_carro, id_motorista) "
            "VALUES ($1, $2, $3, $4) RETURNING id_viagem",
            pqxx::params{dataViagemISO, viagem.cidadeDestino, viagem.veiculoId, viagem.motoristaId});

        int id_viagem = res[0][0].as<int>();

        // Insere os pacientes e acompanhantes na tabela associativa
        for (const auto &pv : viagem.passageiros)
        {
            if (pv.acompanhanteId.has_value())
            {
                transacao.exec(
                    "INSERT INTO Viagem_Pacientes (id_viagem, id_paciente, id_acompanhante) VALUES ($1, $2, $3)",
                    pqxx::params{id_viagem, pv.pacienteId, pv.acompanhanteId.value()});
            }
            else
            {
                transacao.exec(
                    "INSERT INTO Viagem_Pacientes (id_viagem, id_paciente, id_acompanhante) VALUES ($1, $2, NULL)",
                    pqxx::params{id_viagem, pv.pacienteId});
            }
        }

        transacao.commit();
        std::cout << "Viagem cadastrada com sucesso! ID da Viagem: " << id_viagem << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao cadastrar viagem: " << e.what() << std::endl;
        throw std::runtime_error("Não foi possível cadastrar a viagem. Verifique os dados e tente novamente.");
    }
}

std::vector<HistoricoPacienteItem> ViagemRepository::gerarRelatorioHistoricoPaciente(const std::string &cpfPaciente)
{
    std::vector<HistoricoPacienteItem> resultado;

    try
    {
        db.exigirConexao();
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

        pqxx::result res = transacao.exec(sql, pqxx::params{cpfPaciente});

        for (auto row : res)
        {
            HistoricoPacienteItem item;
            item.dataViagem = DataUtils::paraBR(row["data_viagem"].c_str());
            item.cidadeDestino = row["cidade_destino"].c_str();
            item.motorista = row["motorista"].c_str();
            item.veiculoModelo = row["veiculo_modelo"].c_str();
            item.veiculoPlaca = row["veiculo_placa"].c_str();

            if (!row["acompanhante"].is_null())
            {
                item.acompanhante = std::string(row["acompanhante"].c_str());
            }

            resultado.push_back(item);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao gerar histórico: " << e.what() << std::endl;
        throw std::runtime_error("Erro ao gerar o histórico, tente novamente!");
    }

    return resultado;
}

VolumePassageirosResultado ViagemRepository::gerarRelatorioVolumePassageiros(const std::string &dataInicio, const std::string &dataFim)
{
    VolumePassageirosResultado resultado;

    try
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        // dataInicio/dataFim chegam em "DD-MM-YYYY"; convertemos para ISO
        // para comparar corretamente contra a coluna DATE no BETWEEN.
        std::string dataInicioISO = DataUtils::paraISO(dataInicio);
        std::string dataFimISO = DataUtils::paraISO(dataFim);

        std::string sql =
            "SELECT "
            "(SELECT COUNT(*) FROM Viagem_Pacientes vp JOIN Viagens v ON vp.id_viagem = v.id_viagem WHERE v.data_viagem BETWEEN $1 AND $2) AS total_pacientes, "
            "(SELECT COUNT(id_acompanhante) FROM Viagem_Pacientes vp JOIN Viagens v ON vp.id_viagem = v.id_viagem WHERE v.data_viagem BETWEEN $1 AND $2) AS total_acompanhantes";

        pqxx::result res = transacao.exec(sql, pqxx::params{dataInicioISO, dataFimISO});

        resultado.totalPacientes = res[0]["total_pacientes"].as<int>();
        resultado.totalAcompanhantes = res[0]["total_acompanhantes"].as<int>();
        resultado.totalPessoas = resultado.totalPacientes + resultado.totalAcompanhantes;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao calcular volume de passageiros: " << e.what() << std::endl;
        throw std::runtime_error("Erro ao calcular o volume de passageiros, tente novamente!");
    }

    return resultado;
}

std::vector<MapaViagemItem> ViagemRepository::gerarMapaViagemDiario(const std::string &dataViagem)
{
    std::vector<MapaViagemItem> resultado;

    try
    {
        db.exigirConexao();
        pqxx::work transacao(*db.getConexao());

        // dataViagem chega em "DD-MM-YYYY"; convertemos para ISO para
        // comparar corretamente contra a coluna DATE.
        std::string dataViagemISO = DataUtils::paraISO(dataViagem);

        std::string sql =
            "SELECT v.id_viagem, c.placa, c.modelo, m.nome AS motorista, v.cidade_destino, "
            "p.nome AS paciente, p.telefone AS paciente_telefone, a.nome AS acompanhante "
            "FROM Viagens v "
            "JOIN Carros c ON v.id_carro = c.id_carro "
            "JOIN Motoristas m ON v.id_motorista = m.id_motorista "
            "JOIN Viagem_Pacientes vp ON v.id_viagem = vp.id_viagem "
            "JOIN Pacientes p ON vp.id_paciente = p.id_paciente "
            "LEFT JOIN Acompanhantes a ON vp.id_acompanhante = a.id_acompanhante "
            "WHERE v.data_viagem = $1 "
            "ORDER BY v.id_viagem";

        pqxx::result res = transacao.exec(sql, pqxx::params{dataViagemISO});

        int idViagemAtual = -1;

        for (auto row : res)
        {
            int idViagem = row["id_viagem"].as<int>();

            // Nova viagem encontrada no resultado -> cria um novo item no
            // vetor. Usamos resultado.back() (em vez de guardar um ponteiro
            // pro item) para não sofrer com o vector realocando memória e
            // invalidando um ponteiro anterior a cada push_back().
            if (idViagem != idViagemAtual)
            {
                MapaViagemItem item;
                item.viagemId = idViagem;
                item.veiculoPlaca = row["placa"].c_str();
                item.veiculoModelo = row["modelo"].c_str();
                item.motorista = row["motorista"].c_str();
                item.cidadeDestino = row["cidade_destino"].c_str();
                resultado.push_back(item);
                idViagemAtual = idViagem;
            }

            MapaViagemPassageiro passageiro;
            passageiro.pacienteNome = row["paciente"].c_str();

            if (!row["paciente_telefone"].is_null())
            {
                passageiro.pacienteTelefone = std::string(row["paciente_telefone"].c_str());
            }
            if (!row["acompanhante"].is_null())
            {
                passageiro.acompanhanteNome = std::string(row["acompanhante"].c_str());
            }

            resultado.back().passageiros.push_back(passageiro);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao gerar mapa de viagem diário: " << e.what() << std::endl;
        throw std::runtime_error("Erro ao gerar o mapa de viagem diário, tente novamente!");
    }

    return resultado;
}