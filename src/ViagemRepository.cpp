#include "ViagemRepository.h"
#include "DataUtils.h"

#include <pqxx/pqxx>
#include <set>

namespace
{
    void validarViagem(const Viagem &viagem)
    {
        if (viagem.dataViagem.empty() || viagem.cidadeDestino.empty() || viagem.veiculoId <= 0 || viagem.motoristaId <= 0)
            throw RegraNegocioException("Preencha data, destino, veículo e motorista da viagem.");
        if (viagem.passageiros.empty())
            throw RegraNegocioException("Inclua ao menos um paciente na viagem.");

        std::set<int> pacientes;
        for (const auto &passageiro : viagem.passageiros)
        {
            if (passageiro.pacienteId <= 0)
                throw RegraNegocioException("Há um paciente inválido na viagem.");
            if (!pacientes.insert(passageiro.pacienteId).second)
                throw RegraNegocioException("O mesmo paciente não pode ser incluído duas vezes na viagem.");
            if (passageiro.acompanhanteId.has_value() && passageiro.acompanhanteId.value() <= 0)
                throw RegraNegocioException("Há um acompanhante inválido na viagem.");
        }
    }

    void inserirPassageiros(pqxx::work &transacao, int idViagem, const std::vector<PassageiroDaViagem> &passageiros)
    {
        for (const auto &passageiro : passageiros)
        {
            transacao.exec(
                "INSERT INTO Viagem_Pacientes (id_viagem, id_paciente, id_acompanhante) VALUES ($1, $2, $3)",
                pqxx::params{idViagem, passageiro.pacienteId, passageiro.acompanhanteId});
        }
    }
}

ViagemRepository::ViagemRepository(Database &database) : db(database) {}

int ViagemRepository::cadastrarViagem(const Viagem &viagem)
{
    validarViagem(viagem);
    db.exigirConexao();
    pqxx::work transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "INSERT INTO Viagens (data_viagem, cidade_destino, id_carro, id_motorista) "
        "VALUES ($1, $2, $3, $4) RETURNING id_viagem",
        pqxx::params{DataUtils::paraISO(viagem.dataViagem), viagem.cidadeDestino, viagem.veiculoId, viagem.motoristaId});
    const int idViagem = resultado[0][0].as<int>();
    inserirPassageiros(transacao, idViagem, viagem.passageiros);
    transacao.commit();
    return idViagem;
}

bool ViagemRepository::atualizarViagem(const Viagem &viagem)
{
    validarViagem(viagem);
    if (viagem.id <= 0)
        throw RegraNegocioException("A viagem selecionada é inválida.");

    db.exigirConexao();
    pqxx::work transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "UPDATE Viagens SET data_viagem = $1, cidade_destino = $2, id_carro = $3, id_motorista = $4 "
        "WHERE id_viagem = $5 RETURNING id_viagem",
        pqxx::params{DataUtils::paraISO(viagem.dataViagem), viagem.cidadeDestino, viagem.veiculoId, viagem.motoristaId, viagem.id});
    if (resultado.empty())
        return false;

    transacao.exec("DELETE FROM Viagem_Pacientes WHERE id_viagem = $1", pqxx::params{viagem.id});
    inserirPassageiros(transacao, viagem.id, viagem.passageiros);
    transacao.commit();
    return true;
}

bool ViagemRepository::deletarViagem(int idViagem)
{
    db.exigirConexao();
    pqxx::work transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "DELETE FROM Viagens WHERE id_viagem = $1 RETURNING id_viagem", pqxx::params{idViagem});
    transacao.commit();
    return !resultado.empty();
}

std::optional<ViagemDetalhe> ViagemRepository::buscarPorId(int idViagem)
{
    db.exigirConexao();
    pqxx::read_transaction transacao(*db.getConexao());
    const pqxx::result cabecalho = transacao.exec(
        "SELECT id_viagem, data_viagem, cidade_destino, id_carro, id_motorista FROM Viagens WHERE id_viagem = $1",
        pqxx::params{idViagem});
    if (cabecalho.empty())
        return std::nullopt;

    ViagemDetalhe detalhe;
    detalhe.viagem.id = cabecalho[0]["id_viagem"].as<int>();
    detalhe.viagem.dataViagem = DataUtils::paraBR(cabecalho[0]["data_viagem"].c_str());
    detalhe.viagem.cidadeDestino = cabecalho[0]["cidade_destino"].c_str();
    detalhe.viagem.veiculoId = cabecalho[0]["id_carro"].as<int>();
    detalhe.viagem.motoristaId = cabecalho[0]["id_motorista"].as<int>();

    const pqxx::result passageiros = transacao.exec(
        "SELECT vp.id_paciente, p.nome AS paciente_nome, p.cpf AS paciente_cpf, "
        "vp.id_acompanhante, a.nome AS acompanhante_nome, a.cpf AS acompanhante_cpf "
        "FROM Viagem_Pacientes vp "
        "JOIN Pacientes p ON p.id_paciente = vp.id_paciente "
        "LEFT JOIN Acompanhantes a ON a.id_acompanhante = vp.id_acompanhante "
        "WHERE vp.id_viagem = $1 ORDER BY p.nome, p.cpf",
        pqxx::params{idViagem});

    for (const auto &linha : passageiros)
    {
        PassageiroViagemDetalhe passageiro;
        passageiro.relacao.pacienteId = linha["id_paciente"].as<int>();
        passageiro.pacienteNome = linha["paciente_nome"].c_str();
        passageiro.pacienteCpf = linha["paciente_cpf"].c_str();
        if (!linha["id_acompanhante"].is_null())
        {
            passageiro.relacao.acompanhanteId = linha["id_acompanhante"].as<int>();
            passageiro.acompanhanteNome = std::string(linha["acompanhante_nome"].c_str());
            passageiro.acompanhanteCpf = std::string(linha["acompanhante_cpf"].c_str());
        }
        detalhe.viagem.passageiros.push_back(passageiro.relacao);
        detalhe.passageiros.push_back(passageiro);
    }
    return detalhe;
}

std::vector<ViagemResumo> ViagemRepository::listarPorData(const std::string &dataViagem)
{
    db.exigirConexao();
    pqxx::read_transaction transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "SELECT v.id_viagem, v.data_viagem, v.cidade_destino, c.placa, m.nome AS motorista, "
        "COUNT(vp.id_paciente) AS total_pacientes, COUNT(vp.id_acompanhante) AS total_acompanhantes "
        "FROM Viagens v JOIN Carros c ON c.id_carro = v.id_carro "
        "JOIN Motoristas m ON m.id_motorista = v.id_motorista "
        "LEFT JOIN Viagem_Pacientes vp ON vp.id_viagem = v.id_viagem "
        "WHERE v.data_viagem = $1 "
        "GROUP BY v.id_viagem, v.data_viagem, v.cidade_destino, c.placa, m.nome "
        "ORDER BY v.cidade_destino, v.id_viagem",
        pqxx::params{DataUtils::paraISO(dataViagem)});

    std::vector<ViagemResumo> viagens;
    viagens.reserve(resultado.size());
    for (const auto &linha : resultado)
    {
        ViagemResumo viagem;
        viagem.id = linha["id_viagem"].as<int>();
        viagem.dataViagem = DataUtils::paraBR(linha["data_viagem"].c_str());
        viagem.cidadeDestino = linha["cidade_destino"].c_str();
        viagem.veiculoPlaca = linha["placa"].c_str();
        viagem.motoristaNome = linha["motorista"].c_str();
        viagem.totalPacientes = linha["total_pacientes"].as<int>();
        viagem.totalAcompanhantes = linha["total_acompanhantes"].as<int>();
        viagens.push_back(viagem);
    }
    return viagens;
}

std::vector<HistoricoPacienteItem> ViagemRepository::gerarRelatorioHistoricoPaciente(const std::string &cpfPaciente)
{
    db.exigirConexao();
    pqxx::read_transaction transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "SELECT v.data_viagem, v.cidade_destino, m.nome AS motorista, c.modelo AS veiculo_modelo, "
        "c.placa AS veiculo_placa, a.nome AS acompanhante "
        "FROM Viagem_Pacientes vp JOIN Pacientes p ON p.id_paciente = vp.id_paciente "
        "JOIN Viagens v ON v.id_viagem = vp.id_viagem JOIN Carros c ON c.id_carro = v.id_carro "
        "JOIN Motoristas m ON m.id_motorista = v.id_motorista "
        "LEFT JOIN Acompanhantes a ON a.id_acompanhante = vp.id_acompanhante "
        "WHERE p.cpf = $1 ORDER BY v.data_viagem DESC, v.id_viagem DESC",
        pqxx::params{cpfPaciente});

    std::vector<HistoricoPacienteItem> itens;
    itens.reserve(resultado.size());
    for (const auto &linha : resultado)
    {
        HistoricoPacienteItem item;
        item.dataViagem = DataUtils::paraBR(linha["data_viagem"].c_str());
        item.cidadeDestino = linha["cidade_destino"].c_str();
        item.motorista = linha["motorista"].c_str();
        item.veiculoModelo = linha["veiculo_modelo"].c_str();
        item.veiculoPlaca = linha["veiculo_placa"].c_str();
        if (!linha["acompanhante"].is_null()) item.acompanhante = std::string(linha["acompanhante"].c_str());
        itens.push_back(item);
    }
    return itens;
}

VolumePassageirosResultado ViagemRepository::gerarRelatorioVolumePassageiros(const std::string &dataInicio, const std::string &dataFim)
{
    db.exigirConexao();
    pqxx::read_transaction transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "SELECT COUNT(vp.id_paciente) AS total_pacientes, COUNT(vp.id_acompanhante) AS total_acompanhantes "
        "FROM Viagens v LEFT JOIN Viagem_Pacientes vp ON vp.id_viagem = v.id_viagem "
        "WHERE v.data_viagem BETWEEN $1 AND $2",
        pqxx::params{DataUtils::paraISO(dataInicio), DataUtils::paraISO(dataFim)});
    VolumePassageirosResultado volume;
    volume.totalPacientes = resultado[0]["total_pacientes"].as<int>();
    volume.totalAcompanhantes = resultado[0]["total_acompanhantes"].as<int>();
    volume.totalPessoas = volume.totalPacientes + volume.totalAcompanhantes;
    return volume;
}

std::vector<MapaViagemItem> ViagemRepository::gerarMapaViagemDiario(const std::string &dataViagem)
{
    db.exigirConexao();
    pqxx::read_transaction transacao(*db.getConexao());
    const pqxx::result resultado = transacao.exec(
        "SELECT v.id_viagem, c.placa, c.modelo, m.nome AS motorista, v.cidade_destino, "
        "p.nome AS paciente, p.telefone AS paciente_telefone, a.nome AS acompanhante "
        "FROM Viagens v JOIN Carros c ON c.id_carro = v.id_carro "
        "JOIN Motoristas m ON m.id_motorista = v.id_motorista "
        "JOIN Viagem_Pacientes vp ON vp.id_viagem = v.id_viagem "
        "JOIN Pacientes p ON p.id_paciente = vp.id_paciente "
        "LEFT JOIN Acompanhantes a ON a.id_acompanhante = vp.id_acompanhante "
        "WHERE v.data_viagem = $1 ORDER BY v.id_viagem, p.nome, p.cpf",
        pqxx::params{DataUtils::paraISO(dataViagem)});

    std::vector<MapaViagemItem> mapa;
    int idAtual = -1;
    for (const auto &linha : resultado)
    {
        const int idViagem = linha["id_viagem"].as<int>();
        if (idViagem != idAtual)
        {
            MapaViagemItem item;
            item.viagemId = idViagem;
            item.veiculoPlaca = linha["placa"].c_str();
            item.veiculoModelo = linha["modelo"].c_str();
            item.motorista = linha["motorista"].c_str();
            item.cidadeDestino = linha["cidade_destino"].c_str();
            mapa.push_back(item);
            idAtual = idViagem;
        }
        MapaViagemPassageiro passageiro;
        passageiro.pacienteNome = linha["paciente"].c_str();
        if (!linha["paciente_telefone"].is_null()) passageiro.pacienteTelefone = std::string(linha["paciente_telefone"].c_str());
        if (!linha["acompanhante"].is_null()) passageiro.acompanhanteNome = std::string(linha["acompanhante"].c_str());
        mapa.back().passageiros.push_back(passageiro);
    }
    return mapa;
}
