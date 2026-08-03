#pragma once
#include <QWidget>
#include "AcompanhanteRepository.h"
#include "Motoristarepository.h"
#include "PacienteRepository.h"
#include "VeiculoRepository.h"
#include "ViagemRepository.h"
namespace Ui
{
    class ViewViagens;
}
class ViewViagens : public QWidget
{
    Q_OBJECT
public:
    explicit ViewViagens(Database &db, QWidget *parent = nullptr);
    ~ViewViagens();
    void prepararTela();
signals:
    void voltarSolicitado();
private slots:
    void carregarRecursos();
    void buscarPaciente();
    void buscarAcompanhante();
    void adicionarPassageiro();
    void removerPassageiro();
    void salvar();
    void listar();
    void carregarSelecionada(int, int);
    void excluirViagem();
    void limpar();

private:
    Ui::ViewViagens *ui;
    PacienteRepository pacientes;
    AcompanhanteRepository acompanhantes;
    MotoristaRepository motoristas;
    VeiculoRepository veiculos;
    ViagemRepository viagens;
    std::optional<Paciente> pacientePendente;
    std::optional<Acompanhante> acompanhantePendente;
    std::vector<PassageiroViagemDetalhe> passageiros;
    int viagemId = 0;
    void configurarTabelas();
    void desenharPassageiros();
    Viagem montarViagem() const;
    void mensagem(const QString &);
    void erro(const QString &);
    void erroSql(const pqxx::sql_error &, const QString &acao);
};
