#include "MainWindow.h"
#include "AppTheme.h"
#include "ViewCadastros.h"
#include "ViewInicio.h"
#include "ViewPacientesMenu.h"
#include "ViewRelatorios.h"
#include "ViewViagens.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(Database &db, QWidget *p) : QMainWindow(p), m_db(db)
{
    setWindowTitle("Sistema de Logística — Prefeitura de Piranga");
    setWindowIcon(AppTheme::appIcon());
    resize(1120, 800);
    try
    {
        m_db.exigirConexao();
        statusBar()->showMessage("Conectado ao PostgreSQL", 5000);
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Banco indisponível", QString("Não foi possível conectar ao banco.\n%1").arg(e.what()));
    }
    configurarInterface();
}

QWidget *MainWindow::construirCabecalho()
{
    auto *cabecalho = new QWidget(this);
    cabecalho->setObjectName("appHeaderBar");
    cabecalho->setFixedHeight(64);

    auto *layout = new QHBoxLayout(cabecalho);
    layout->setContentsMargins(22, 8, 22, 8);
    layout->setSpacing(14);

    auto *logo = new QLabel(cabecalho);
    logo->setPixmap(AppTheme::logoPixmap(48));
    logo->setFixedSize(96, 48);
    logo->setScaledContents(false);
    layout->addWidget(logo, 0, Qt::AlignVCenter);

    auto *blocoTitulo = new QWidget(cabecalho);
    auto *layoutTitulo = new QVBoxLayout(blocoTitulo);
    layoutTitulo->setContentsMargins(0, 0, 0, 0);
    layoutTitulo->setSpacing(1);
    auto *titulo = new QLabel("Sistema de Logística de Saúde", blocoTitulo);
    titulo->setObjectName("appHeaderTitulo");
    auto *subtitulo = new QLabel("Prefeitura de Piranga · Tratamento Fora de Domicílio", blocoTitulo);
    subtitulo->setObjectName("appHeaderSubtitulo");
    layoutTitulo->addWidget(titulo);
    layoutTitulo->addWidget(subtitulo);
    layout->addWidget(blocoTitulo, 0, Qt::AlignVCenter);

    layout->addStretch();

    auto *selo = new QLabel("Developed by Kayke", cabecalho);
    selo->setObjectName("appHeaderSelo");
    layout->addWidget(selo, 0, Qt::AlignVCenter);

    return cabecalho;
}

void MainWindow::configurarInterface()
{
    auto *containerCentral = new QWidget(this);
    auto *layoutCentral = new QVBoxLayout(containerCentral);
    layoutCentral->setContentsMargins(0, 0, 0, 0);
    layoutCentral->setSpacing(0);
    layoutCentral->addWidget(construirCabecalho());

    m_stackTelas = new QStackedWidget(containerCentral);
    layoutCentral->addWidget(m_stackTelas);
    setCentralWidget(containerCentral);

    auto *rodapeStatus = new QLabel("Developed by Kayke", this);
    statusBar()->addPermanentWidget(rodapeStatus);

    m_telaInicio = new ViewInicio(this);
    m_telaMenuPacientes = new ViewPacientesMenu(this);
    m_telaPacientesCadastrar = new ViewPacientes(m_db, ModoPaciente::Cadastrar, this);
    m_telaPacientesAtualizar = new ViewPacientes(m_db, ModoPaciente::Atualizar, this);
    m_telaPacientesExcluir = new ViewPacientes(m_db, ModoPaciente::Excluir, this);
    m_telaPacientesConsultar = new ViewPacientes(m_db, ModoPaciente::Consultar, this);
    m_telaCadastros = new ViewCadastros(m_db, this);
    m_telaViagens = new ViewViagens(m_db, this);
    m_telaRelatorios = new ViewRelatorios(m_db, this);
    for (auto *w : {static_cast<QWidget *>(m_telaInicio), static_cast<QWidget *>(m_telaMenuPacientes), static_cast<QWidget *>(m_telaPacientesCadastrar), static_cast<QWidget *>(m_telaPacientesAtualizar), static_cast<QWidget *>(m_telaPacientesExcluir), static_cast<QWidget *>(m_telaPacientesConsultar), static_cast<QWidget *>(m_telaCadastros), static_cast<QWidget *>(m_telaViagens), static_cast<QWidget *>(m_telaRelatorios)})
        m_stackTelas->addWidget(w);
    connect(m_telaInicio, &ViewInicio::pacientesSelecionado, this, &MainWindow::abrirMenuPacientes);
    connect(m_telaInicio, &ViewInicio::viagensSelecionado, this, &MainWindow::abrirViagens);
    connect(m_telaInicio, &ViewInicio::relatoriosSelecionado, this, &MainWindow::abrirRelatorios);
    connect(m_telaInicio, &ViewInicio::cadastrosSelecionado, this, &MainWindow::abrirCadastros);
    connect(m_telaMenuPacientes, &ViewPacientesMenu::opcaoEscolhida, this, &MainWindow::abrirTelaPaciente);
    connect(m_telaMenuPacientes, &ViewPacientesMenu::voltarSolicitado, this, &MainWindow::voltarParaInicio);
    for (auto *w : {m_telaPacientesCadastrar, m_telaPacientesAtualizar, m_telaPacientesExcluir, m_telaPacientesConsultar})
        connect(w, &ViewPacientes::voltarSolicitado, this, &MainWindow::abrirMenuPacientes);
    connect(m_telaCadastros, &ViewCadastros::voltarSolicitado, this, &MainWindow::voltarParaInicio);
    connect(m_telaViagens, &ViewViagens::voltarSolicitado, this, &MainWindow::voltarParaInicio);
    connect(m_telaRelatorios, &ViewRelatorios::voltarSolicitado, this, &MainWindow::voltarParaInicio);
    m_stackTelas->setCurrentWidget(m_telaInicio);
}
void MainWindow::abrirMenuPacientes() { m_stackTelas->setCurrentWidget(m_telaMenuPacientes); }
void MainWindow::voltarParaInicio() { m_stackTelas->setCurrentWidget(m_telaInicio); }
void MainWindow::abrirCadastros()
{
    m_telaCadastros->prepararTela();
    m_stackTelas->setCurrentWidget(m_telaCadastros);
}
void MainWindow::abrirViagens()
{
    m_telaViagens->prepararTela();
    m_stackTelas->setCurrentWidget(m_telaViagens);
}
void MainWindow::abrirRelatorios()
{
    m_telaRelatorios->prepararTela();
    m_stackTelas->setCurrentWidget(m_telaRelatorios);
}
void MainWindow::abrirTelaPaciente(ModoPaciente m)
{
    ViewPacientes *w = m == ModoPaciente::Cadastrar ? m_telaPacientesCadastrar : m == ModoPaciente::Atualizar ? m_telaPacientesAtualizar
                                                                             : m == ModoPaciente::Excluir     ? m_telaPacientesExcluir
                                                                                                              : m_telaPacientesConsultar;
    w->prepararTela();
    m_stackTelas->setCurrentWidget(w);
}