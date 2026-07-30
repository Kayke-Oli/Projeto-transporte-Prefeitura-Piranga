#pragma once

#include <QWidget>
#include <QListWidgetItem>
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

signals:
    void voltarSolicitado();

private slots:
    void salvarPaciente();
    void atualizarPaciente();
    void deletarPaciente();
    void buscarPaciente();
    void selecionarResultado(QListWidgetItem *item);

private:
    Ui::ViewPacientes *ui;

    Database &m_db;
    PacienteRepository m_repo;
    ModoPaciente m_modo;

    // Guarda os resultados da última busca por nome, pra mapear o item
    // clicado na lista de volta pro Paciente completo (a lista só mostra
    // texto, não guarda os dados).
    std::vector<Paciente> m_resultadosBusca;

    void configurarMascaras();
    void configurarModo();
    void habilitarCamposEdicao(bool habilitado);
    void preencherCampos(const Paciente &p);
    void limparCamposDados();
};