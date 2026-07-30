#pragma once

#include <QWidget>
#include "Database.h"
#include "PacienteRepository.h"

// 1. Avisa o C++ que existe uma interface desenhada chamada ViewPacientes no Qt
namespace Ui
{
    class ViewPacientes;
}

class ViewPacientes : public QWidget
{
    Q_OBJECT

public:
    explicit ViewPacientes(Database &db, QWidget *parent = nullptr);
    ~ViewPacientes(); // Precisamos do destrutor para limpar a memória do 'ui'

private slots:
    void salvarPaciente();
    void atualizarPaciente();
    void deletarPaciente();

private:
    Ui::ViewPacientes *ui; // O ponteiro mágico que acessa todos os botões e textos

    Database &m_db;
    PacienteRepository m_repo;

    // Criamos uma função separada só para organizar as formatações de texto
    void configurarMascaras();
};