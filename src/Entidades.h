#pragma once
#include <string>
#include <optional>
#include <vector>

struct Veiculo
{
    int id = 0;
    std::string placa;
    std::string modelo;
};

struct Motorista
{
    int id = 0;
    std::string nome;
    std::string cpf;
};

struct Paciente
{
    int id = 0;
    std::string nomeCompleto;
    std::string cpf;
    std::string telefone;
    std::string endereco;
};

struct Acompanhante
{
    int id = 0;
    std::string nomeCompleto;
    std::string cpf;
    std::string telefone;
};

// Representa uma linha da tabela associativa: um paciente embarcado,
// com um acompanhante opcional, dentro do contexto de UMA viagem.
struct PassageiroDaViagem
{
    int pacienteId = 0;
    std::optional<int> acompanhanteId; // regra: no máx. 1 por paciente/viagem
};

struct Viagem
{
    int id = 0;
    std::string dataViagem; // "DD-MM-YYYY"
    std::string cidadeDestino;
    int veiculoId = 0;
    int motoristaId = 0;
    std::vector<PassageiroDaViagem> passageiros;
};

// Projeções de leitura para a tela de viagens. Mantêm os DTOs de domínio
// simples e evitam expor pqxx para a interface.
struct PassageiroViagemDetalhe
{
    PassageiroDaViagem relacao;
    std::string pacienteNome;
    std::string pacienteCpf;
    std::optional<std::string> acompanhanteNome;
    std::optional<std::string> acompanhanteCpf;
};

struct ViagemDetalhe
{
    Viagem viagem;
    std::vector<PassageiroViagemDetalhe> passageiros;
};

struct ViagemResumo
{
    int id = 0;
    std::string dataViagem;
    std::string cidadeDestino;
    std::string veiculoPlaca;
    std::string motoristaNome;
    int totalPacientes = 0;
    int totalAcompanhantes = 0;
};

// DTOs específicos para os relatórios (evitam misturar entidade de domínio com projeção de query)
struct HistoricoPacienteItem
{
    std::string dataViagem;
    std::string cidadeDestino;
    std::string motorista;
    std::string veiculoModelo;
    std::string veiculoPlaca;
    std::optional<std::string> acompanhante;
};

struct VolumePassageirosResultado
{
    int totalPacientes = 0;
    int totalAcompanhantes = 0;
    int totalPessoas = 0;
};

struct MapaViagemPassageiro
{
    std::string pacienteNome;
    std::optional<std::string> pacienteTelefone;
    std::optional<std::string> acompanhanteNome;
};

struct MapaViagemItem
{
    int viagemId = 0;
    std::string veiculoPlaca;
    std::string veiculoModelo;
    std::string motorista;
    std::string cidadeDestino;
    std::vector<MapaViagemPassageiro> passageiros;
};
