#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QTableView>
#include "Database.h"
#include "PacienteRepository.h"

class ViewPacientes : public QWidget
{
    Q_OBJECT

public:
    explicit ViewPacientes(Database &db, QWidget *parent = nullptr);

private slots:
    void salvarPaciente();
    void atualizarPaciente();
    void deletarPaciente();

private:
    Database &m_db;
    PacienteRepository m_repo;

    // Componentes de Input
    QLineEdit *txtNome;
    QLineEdit *txtCpf;
    QLineEdit *txtTelefone;
    QLineEdit *txtEndereco;
    QPushButton *btnSalvar;

    void configurarFormulario();
};