#include "Database.h"
#include "Entidades.h"
#include "PacienteRepository.h"
#include "AcompanhanteRepository.h"
#include "MotoristaRepository.h"
#include "VeiculoRepository.h"
#include "ViagemRepository.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>

// =====================================================================
// Funções auxiliares de entrada de dados
// =====================================================================
// Centralizar a leitura aqui evita repetir tratamento de erro em cada
// fluxo de cadastro, e protege o programa contra entradas inválidas
// (texto onde se espera número) ou EOF (ex.: usuário fecha o terminal
// com Ctrl+D/Ctrl+Z), que travariam o loop de leitura em um laço infinito
// se não fossem tratados.

std::string lerLinha(const std::string &prompt)
{
    std::string valor;
    std::cout << prompt;
    std::getline(std::cin, valor);
    if (std::cin.eof())
    {
        std::cout << "\nEntrada encerrada (EOF). Encerrando o programa.\n";
        std::exit(0);
    }
    return valor;
}

int lerInteiro(const std::string &prompt)
{
    int valor;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> valor)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return valor;
        }

        if (std::cin.eof())
        {
            std::cout << "\nEntrada encerrada (EOF). Encerrando o programa.\n";
            std::exit(0);
        }

        std::cout << "Entrada inválida, digite um número inteiro.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool perguntarSimNao(const std::string &prompt)
{
    std::string resposta = lerLinha(prompt + " (s/n): ");
    return !resposta.empty() && (resposta[0] == 's' || resposta[0] == 'S');
}

// =====================================================================
// Fluxos de cadastro
// =====================================================================

void cadastrarPacienteFluxo(PacienteRepository &repo)
{
    std::cout << "\n-- Cadastro de Paciente --\n";
    Paciente p;
    p.cpf = lerLinha("CPF (somente números): ");
    p.nomeCompleto = lerLinha("Nome completo: ");
    p.telefone = lerLinha("Telefone: ");
    p.endereco = lerLinha("Endereço: ");
    repo.cadastrar(p);
}

void buscarPacientePorCPFFluxo(PacienteRepository &repo)
{
    std::cout << "\n-- Buscar Paciente por CPF --\n";
    std::string cpf = lerLinha("CPF: ");
    auto paciente = repo.buscarPorCPF(cpf);

    if (paciente.has_value())
    {
        std::cout << "Encontrado -> ID: " << paciente->id
                  << " | Nome: " << paciente->nomeCompleto
                  << " | Telefone: " << paciente->telefone
                  << " | Endereço: " << paciente->endereco << "\n";
    }
    else
    {
        std::cout << "Nenhum paciente encontrado com esse CPF.\n";
    }
}

void cadastrarAcompanhanteFluxo(AcompanhanteRepository &repo)
{
    std::cout << "\n-- Cadastro de Acompanhante --\n";
    Acompanhante a;
    a.cpf = lerLinha("CPF (somente números): ");
    a.nomeCompleto = lerLinha("Nome completo: ");
    a.telefone = lerLinha("Telefone: ");
    repo.cadastrar(a);
}

void cadastrarMotoristaFluxo(MotoristaRepository &repo)
{
    std::cout << "\n-- Cadastro de Motorista --\n";
    Motorista m;
    m.nome = lerLinha("Nome completo: ");
    m.cpf = lerLinha("CPF (somente números): ");
    repo.cadastrar(m);
}

void cadastrarVeiculoFluxo(VeiculoRepository &repo)
{
    std::cout << "\n-- Cadastro de Veículo --\n";
    Veiculo v;
    v.placa = lerLinha("Placa: ");
    v.modelo = lerLinha("Modelo: ");
    repo.cadastrar(v);
}

void cadastrarViagemFluxo(ViagemRepository &repo)
{
    std::cout << "\n-- Cadastro de Viagem --\n";
    Viagem viagem;
    viagem.dataViagem = lerLinha("Data da viagem (DD-MM-YYYY): ");
    viagem.cidadeDestino = lerLinha("Cidade de destino: ");
    viagem.veiculoId = lerInteiro("ID do veículo (cadastrado previamente): ");
    viagem.motoristaId = lerInteiro("ID do motorista (cadastrado previamente): ");

    std::cout << "\nAgora informe os pacientes que vão nesta viagem.\n";
    do
    {
        PassageiroDaViagem pv;
        pv.pacienteId = lerInteiro("  ID do paciente (cadastrado previamente): ");

        if (perguntarSimNao("  Este paciente vai com acompanhante?"))
        {
            pv.acompanhanteId = lerInteiro("  ID do acompanhante (cadastrado previamente): ");
        }

        viagem.passageiros.push_back(pv);
    } while (perguntarSimNao("Adicionar outro paciente a esta viagem?"));

    repo.cadastrarViagem(viagem);
}

// =====================================================================
// Fluxos de relatório
// =====================================================================

void relatorioHistoricoFluxo(ViagemRepository &repo)
{
    std::cout << "\n-- Relatório: Histórico de Viagens do Paciente --\n";
    std::string cpf = lerLinha("CPF do paciente: ");
    repo.gerarRelatorioHistoricoPaciente(cpf);
}

void relatorioVolumeFluxo(ViagemRepository &repo)
{
    std::cout << "\n-- Relatório: Volume de Passageiros por Período --\n";
    std::string dataInicio = lerLinha("Data inicial (DD-MM-YYYY): ");
    std::string dataFim = lerLinha("Data final (DD-MM-YYYY): ");
    repo.gerarRelatorioVolumePassageiros(dataInicio, dataFim);
}

void mapaDiarioFluxo(ViagemRepository &repo)
{
    std::cout << "\n-- Relatório: Mapa de Viagens do Dia --\n";
    std::string data = lerLinha("Data (DD-MM-YYYY): ");
    repo.gerarMapaViagemDiario(data);
}

// =====================================================================
// Menu principal
// =====================================================================

void exibirMenu()
{
    std::cout << "\n==============================================\n";
    std::cout << " Sistema de Transporte de Pacientes - TFD\n";
    std::cout << " Prefeitura de Piranga\n";
    std::cout << "==============================================\n";
    std::cout << " 1  - Cadastrar paciente\n";
    std::cout << " 2  - Buscar paciente por CPF\n";
    std::cout << " 3  - Cadastrar acompanhante\n";
    std::cout << " 4  - Cadastrar motorista\n";
    std::cout << " 5  - Cadastrar veículo\n";
    std::cout << " 6  - Cadastrar viagem\n";
    std::cout << " 7  - Relatório: histórico de viagens de um paciente\n";
    std::cout << " 8  - Relatório: volume de passageiros por período\n";
    std::cout << " 9  - Relatório: mapa de viagens do dia\n";
    std::cout << " 0  - Sair\n";
    std::cout << "==============================================\n";
}

int main()
{
    Database db;
    db.conectar();

    // Garante que a conexão está realmente utilizável antes de abrir o
    // menu. Sem essa checagem, qualquer operação abaixo desreferenciaria
    // um ponteiro nulo (getConexao() retornando nullptr) caso a conexão
    // inicial tenha falhado, causando um crash em vez de uma mensagem clara.
    if (!db.garantirConectado())
    {
        std::cerr << "Não foi possível estabelecer conexão com o banco de dados. "
                     "Verifique as variáveis de ambiente (DB_HOST, DB_PORT, DB_NAME, "
                     "DB_USER, DB_PASSWORD) e se o servidor PostgreSQL está acessível.\n";
        return 1;
    }

    PacienteRepository pacienteRepo(db);
    AcompanhanteRepository acompanhanteRepo(db);
    MotoristaRepository motoristaRepo(db);
    VeiculoRepository veiculoRepo(db);
    ViagemRepository viagemRepo(db);

    int opcao = -1;
    do
    {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opção: ");

        switch (opcao)
        {
        case 1:
            cadastrarPacienteFluxo(pacienteRepo);
            break;
        case 2:
            buscarPacientePorCPFFluxo(pacienteRepo);
            break;
        case 3:
            cadastrarAcompanhanteFluxo(acompanhanteRepo);
            break;
        case 4:
            cadastrarMotoristaFluxo(motoristaRepo);
            break;
        case 5:
            cadastrarVeiculoFluxo(veiculoRepo);
            break;
        case 6:
            cadastrarViagemFluxo(viagemRepo);
            break;
        case 7:
            relatorioHistoricoFluxo(viagemRepo);
            break;
        case 8:
            relatorioVolumeFluxo(viagemRepo);
            break;
        case 9:
            mapaDiarioFluxo(viagemRepo);
            break;
        case 0:
            std::cout << "Encerrando o sistema...\n";
            break;
        default:
            std::cout << "Opção inválida. Tente novamente.\n";
            break;
        }
    } while (opcao != 0);

    db.desconectar();
    return 0;
}