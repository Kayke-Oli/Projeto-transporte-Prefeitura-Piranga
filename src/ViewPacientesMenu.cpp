#include "ViewPacientesMenu.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

ViewPacientesMenu::ViewPacientesMenu(QWidget *parent) : QWidget(parent)
{
    configurarInterface();
}

void ViewPacientesMenu::configurarInterface()
{
    auto *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(40, 40, 40, 40);
    layoutPrincipal->setSpacing(30);

    auto *layoutTopo = new QHBoxLayout();
    auto *btnVoltar = new QPushButton("< Voltar", this);
    btnVoltar->setMaximumWidth(100);
    connect(btnVoltar, &QPushButton::clicked, this, &ViewPacientesMenu::voltarSolicitado);
    layoutTopo->addWidget(btnVoltar);
    layoutTopo->addStretch();
    layoutPrincipal->addLayout(layoutTopo);

    auto *titulo = new QLabel("Pacientes - Escolha uma opção", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    layoutPrincipal->addWidget(titulo);
    layoutPrincipal->addStretch();

    auto *layoutOpcoes = new QGridLayout();
    layoutOpcoes->setSpacing(20);

    auto criarBotao = [this](const QString &texto, const QString &cor)
    {
        auto *botao = new QPushButton(texto, this);
        botao->setMinimumSize(180, 120);
        botao->setStyleSheet(QString(
                                 "QPushButton { background-color: %1; color: white; font-size: 15px; "
                                 "font-weight: bold; border-radius: 10px; }"
                                 "QPushButton:hover { background-color: #34495e; }")
                                 .arg(cor));
        return botao;
    };

    auto *btnCadastrar = criarBotao("Cadastrar", "#27ae60");
    auto *btnAtualizar = criarBotao("Atualizar Cadastro", "#2980b9");
    auto *btnExcluir = criarBotao("Excluir Cadastro", "#c0392b");
    auto *btnConsultar = criarBotao("Consultar Cadastro", "#8e44ad");

    connect(btnCadastrar, &QPushButton::clicked, this, [this]
            { emit opcaoEscolhida(ModoPaciente::Cadastrar); });
    connect(btnAtualizar, &QPushButton::clicked, this, [this]
            { emit opcaoEscolhida(ModoPaciente::Atualizar); });
    connect(btnExcluir, &QPushButton::clicked, this, [this]
            { emit opcaoEscolhida(ModoPaciente::Excluir); });
    connect(btnConsultar, &QPushButton::clicked, this, [this]
            { emit opcaoEscolhida(ModoPaciente::Consultar); });

    layoutOpcoes->addWidget(btnCadastrar, 0, 0);
    layoutOpcoes->addWidget(btnAtualizar, 0, 1);
    layoutOpcoes->addWidget(btnExcluir, 1, 0);
    layoutOpcoes->addWidget(btnConsultar, 1, 1);

    layoutPrincipal->addLayout(layoutOpcoes);
    layoutPrincipal->addStretch();
}