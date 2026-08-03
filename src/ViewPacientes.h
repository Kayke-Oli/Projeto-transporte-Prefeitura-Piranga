#pragma once

#include <QWidget>
#include <vector>

#include "Database.h"
#include "PacienteRepository.h"

namespace Ui
{
    class ViewPacientes;
}

enum class ModoPaciente
{
    Cadastrar,
    Atualizar,
    Excluir,
    Consultar
};

class ViewPacientes : public QWidget
{
    Q_OBJECT

public:
    explicit ViewPacientes(Database &db, ModoPaciente modo, QWidget *parent = nullptr);
    ~ViewPacientes();

    // Called whenever this persistent stacked page is reopened.
    void prepararTela();

signals:
    void voltarSolicitado();

private slots:
    void salvarPaciente();
    void atualizarPaciente();
    void deletarPaciente();
    void buscarPaciente();
    void selecionarResultado(int linha, int coluna);
    void limparFormulario();

private:
    Ui::ViewPacientes *ui;

    Database &m_db;
    PacienteRepository m_repo;
    ModoPaciente m_modo;

    std::vector<Paciente> m_resultadosBusca;
    int m_pacienteSelecionadoId = 0;

    void configurarMascaras();
    void configurarModo();
    void habilitarCamposEdicao(bool habilitado);
    void preencherCampos(const Paciente &paciente);
    void selecionarPaciente(const Paciente &paciente);
    void limparCamposDados();
    bool validarDadosPaciente();
    void exibirMensagemValidacao(const QString &mensagem, QWidget *campo);
    void limparMensagemValidacao();
    void mostrarErroBanco(const pqxx::sql_error &erro, const QString &acao);
};
