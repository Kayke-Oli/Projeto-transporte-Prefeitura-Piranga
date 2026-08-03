#pragma once

#include <QMainWindow>
#include <QStackedWidget>

#include "Database.h"
#include "ViewPacientes.h"

class ViewInicio;
class ViewPacientesMenu;
class ViewCadastros;
class ViewViagens;
class ViewRelatorios;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(Database &db, QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    void abrirMenuPacientes();
    void abrirTelaPaciente(ModoPaciente modo);
    void abrirCadastros();
    void abrirViagens();
    void abrirRelatorios();
    void voltarParaInicio();

private:
    Database &m_db;
    QStackedWidget *m_stackTelas{};
    ViewInicio *m_telaInicio{};
    ViewPacientesMenu *m_telaMenuPacientes{};
    ViewPacientes *m_telaPacientesCadastrar{};
    ViewPacientes *m_telaPacientesAtualizar{};
    ViewPacientes *m_telaPacientesExcluir{};
    ViewPacientes *m_telaPacientesConsultar{};
    ViewCadastros *m_telaCadastros{};
    ViewViagens *m_telaViagens{};
    ViewRelatorios *m_telaRelatorios{};

    void configurarInterface();
};
