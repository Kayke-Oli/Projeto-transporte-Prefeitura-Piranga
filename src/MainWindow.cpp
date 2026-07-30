#include "MainWindow.h"
#include "ViewInicio.h"
#include "ViewPacientesMenu.h"
#include <QStatusBar>
#include <QMessageBox>

MainWindow::MainWindow(Database &db, QWidget *parent)
    : QMainWindow(parent), m_db(db)
{
    setWindowTitle("Sistema de Logística - Prefeitura");
    resize(1024, 768);

    try
    {
        m_db.exigirConexao();
        statusBar()->showMessage("Status: Conectado ao Servidor PostgreSQL", 5000);
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Erro Crítico", QString("Falha de rede:\n") + e.what());
    }

    configurarInterface();
}

MainWindow::~MainWindow() {}

void MainWindow::configurarInterface()
{
    m_stackTelas = new QStackedWidget(this);
    setCentralWidget(m_stackTelas);

    m_telaInicio = new ViewInicio(this);
    m_telaMenuPacientes = new ViewPacientesMenu(this);
    m_telaPacientesCadastrar = new ViewPacientes(m_db, ModoPaciente::Cadastrar, this);
    m_telaPacientesAtualizar = new ViewPacientes(m_db, ModoPaciente::Atualizar, this);
    m_telaPacientesExcluir = new ViewPacientes(m_db, ModoPaciente::Excluir, this);
    m_telaPacientesConsultar = new ViewPacientes(m_db, ModoPaciente::Consultar, this);

    m_stackTelas->addWidget(m_telaInicio);
    m_stackTelas->addWidget(m_telaMenuPacientes);
    m_stackTelas->addWidget(m_telaPacientesCadastrar);
    m_stackTelas->addWidget(m_telaPacientesAtualizar);
    m_stackTelas->addWidget(m_telaPacientesExcluir);
    m_stackTelas->addWidget(m_telaPacientesConsultar);

    connect(m_telaInicio, &ViewInicio::pacientesSelecionado, this, &MainWindow::abrirMenuPacientes);
    connect(m_telaInicio, &ViewInicio::viagensSelecionado, this, [this]
            { QMessageBox::information(this, "Em construção", "A tela de Viagens ainda está sendo desenvolvida."); });
    connect(m_telaInicio, &ViewInicio::relatoriosSelecionado, this, [this]
            { QMessageBox::information(this, "Em construção", "A tela de Relatórios ainda está sendo desenvolvida."); });

    connect(m_telaMenuPacientes, &ViewPacientesMenu::opcaoEscolhida, this, &MainWindow::abrirTelaPaciente);
    connect(m_telaMenuPacientes, &ViewPacientesMenu::voltarSolicitado, this, &MainWindow::voltarParaInicio);

    connect(m_telaPacientesCadastrar, &ViewPacientes::voltarSolicitado, this, &MainWindow::abrirMenuPacientes);
    connect(m_telaPacientesAtualizar, &ViewPacientes::voltarSolicitado, this, &MainWindow::abrirMenuPacientes);
    connect(m_telaPacientesExcluir, &ViewPacientes::voltarSolicitado, this, &MainWindow::abrirMenuPacientes);
    connect(m_telaPacientesConsultar, &ViewPacientes::voltarSolicitado, this, &MainWindow::abrirMenuPacientes);

    m_stackTelas->setCurrentWidget(m_telaInicio);
}

void MainWindow::abrirMenuPacientes() { m_stackTelas->setCurrentWidget(m_telaMenuPacientes); }
void MainWindow::voltarParaInicio() { m_stackTelas->setCurrentWidget(m_telaInicio); }

void MainWindow::abrirTelaPaciente(ModoPaciente modo)
{
    switch (modo)
    {
    case ModoPaciente::Cadastrar:
        m_stackTelas->setCurrentWidget(m_telaPacientesCadastrar);
        break;
    case ModoPaciente::Atualizar:
        m_stackTelas->setCurrentWidget(m_telaPacientesAtualizar);
        break;
    case ModoPaciente::Excluir:
        m_stackTelas->setCurrentWidget(m_telaPacientesExcluir);
        break;
    case ModoPaciente::Consultar:
        m_stackTelas->setCurrentWidget(m_telaPacientesConsultar);
        break;
    }
}