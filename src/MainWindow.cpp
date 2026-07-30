#include "MainWindow.h"
#include "ViewPacientes.h"
#include <QStatusBar>
#include <QMessageBox>

MainWindow::MainWindow(Database &db, QWidget *parent)
    : QMainWindow(parent), m_db(db)
{
    setWindowTitle("Sistema de Logística - Prefeitura");
    resize(1024, 768); // Resolução padrão segura

    // Testa a conexão ao abrir o sistema
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
    configurarEstilo();
}

MainWindow::~MainWindow()
{
    // O Qt destrói automaticamente tudo que está atrelado ao 'this'
}

void MainWindow::configurarInterface()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0); // Remove bordas brancas
    m_mainLayout->setSpacing(0);

    // Menu Lateral (Navegação Segura)
    m_menuLateral = new QListWidget(this);
    m_menuLateral->setFixedWidth(200);
    m_menuLateral->addItem("Pacientes");
    m_menuLateral->addItem("Viagens");
    m_menuLateral->addItem("Relatórios");

    // Gerenciador de Telas
    m_stackTelas = new QStackedWidget(this);

    // Instancia as telas passando o banco de dados
    m_telaPacientes = new ViewPacientes(m_db, this);
    // m_telaViagens = new ViewViagens(m_db, this); // Exemplo futuro

    m_stackTelas->addWidget(m_telaPacientes);
    // m_stackTelas->addWidget(m_telaViagens);

    // Monta o layout: Menu na esquerda, telas na direita
    m_mainLayout->addWidget(m_menuLateral);
    m_mainLayout->addWidget(m_stackTelas);

    connect(m_menuLateral, &QListWidget::currentRowChanged, this, &MainWindow::alterarTela);
}

void MainWindow::alterarTela(int index)
{
    if (index >= 0 && index < m_stackTelas->count())
    {
        m_stackTelas->setCurrentIndex(index);
    }
}

void MainWindow::configurarEstilo()
{
    // Aplica CSS nativo do Qt para evitar desalinhamento visual
    this->setStyleSheet(R"(
        QListWidget {
            background-color: #2c3e50;
            color: white;
            font-size: 14px;
            border: none;
            padding-top: 10px;
        }
        QListWidget::item {
            padding: 15px;
            border-bottom: 1px solid #34495e;
        }
        QListWidget::item:selected {
            background-color: #3498db;
            font-weight: bold;
        }
    )");
}