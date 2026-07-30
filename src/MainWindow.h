#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include "Database.h"
#include "ViewPacientes.h" // para ModoPaciente

class ViewInicio;
class ViewPacientesMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Database &db, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void abrirMenuPacientes();
    void abrirTelaPaciente(ModoPaciente modo);
    void voltarParaInicio();

private:
    Database &m_db;
    QStackedWidget *m_stackTelas;

    ViewInicio *m_telaInicio;
    ViewPacientesMenu *m_telaMenuPacientes;
    ViewPacientes *m_telaPacientesCadastrar;
    ViewPacientes *m_telaPacientesAtualizar;
    ViewPacientes *m_telaPacientesExcluir;
    ViewPacientes *m_telaPacientesConsultar;

    void configurarInterface();
};