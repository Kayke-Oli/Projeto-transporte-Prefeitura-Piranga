#include "ViewPacientesMenu.h"
#include "AppTheme.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
        QPushButton *criarOpcao(QWidget *pai, const QString &emoji, const QString &titulo,
                                const QString &descricao, const QString &cor, const QString &corHover)
        {
                auto *botao = new QPushButton(pai);
                botao->setCursor(Qt::PointingHandCursor);
                botao->setMinimumSize(230, 138);

                auto *layout = new QVBoxLayout(botao);
                layout->setContentsMargins(20, 18, 18, 16);
                layout->setSpacing(5);

                auto *lblEmoji = new QLabel(emoji, botao);
                lblEmoji->setStyleSheet("font-size:26px; background:transparent;");
                lblEmoji->setAttribute(Qt::WA_TransparentForMouseEvents);

                auto *lblTitulo = new QLabel(titulo, botao);
                lblTitulo->setStyleSheet("font-size:15.5px; font-weight:800; color:#0B2545; background:transparent;");
                lblTitulo->setAttribute(Qt::WA_TransparentForMouseEvents);

                auto *lblDescricao = new QLabel(descricao, botao);
                lblDescricao->setWordWrap(true);
                lblDescricao->setStyleSheet("font-size:11.5px; font-weight:600; color:#5A6B7A; background:transparent;");
                lblDescricao->setAttribute(Qt::WA_TransparentForMouseEvents);

                layout->addWidget(lblEmoji);
                layout->addWidget(lblTitulo);
                layout->addWidget(lblDescricao);
                layout->addStretch();

                botao->setStyleSheet(QString(R"(
            QPushButton {
                background: #ffffff;
                border: 1px solid #DCE4EC;
                border-top: 4px solid %1;
                border-radius: 14px;
                text-align: left;
            }
            QPushButton:hover { border-top: 4px solid %2; background: #FAFCFE; }
            QPushButton:pressed { background: #F1F5F9; }
        )")
                                         .arg(cor, corHover));

                // Sem QGraphicsDropShadowEffect de propósito: em Windows com escala
                // de tela fracionária esse efeito borra o conteúdo interno do botão.
                // A borda colorida acima já dá a sensação de profundidade.

                return botao;
        }
}

ViewPacientesMenu::ViewPacientesMenu(QWidget *parent) : QWidget(parent)
{
        configurarInterface();
}

void ViewPacientesMenu::configurarInterface()
{
        auto *layoutPrincipal = new QVBoxLayout(this);
        layoutPrincipal->setContentsMargins(48, 32, 48, 32);
        layoutPrincipal->setSpacing(26);

        auto *layoutTopo = new QHBoxLayout();
        auto *btnVoltar = new QPushButton(QString::fromUtf8("\xE2\x86\x90  Voltar"), this);
        btnVoltar->setObjectName("btnVoltar");
        btnVoltar->setCursor(Qt::PointingHandCursor);
        btnVoltar->setMaximumWidth(110);
        connect(btnVoltar, &QPushButton::clicked, this, &ViewPacientesMenu::voltarSolicitado);
        layoutTopo->addWidget(btnVoltar);
        layoutTopo->addStretch();
        layoutPrincipal->addLayout(layoutTopo);

        auto *bloco = new QWidget(this);
        bloco->setStyleSheet("background: transparent;");
        auto *layoutBloco = new QVBoxLayout(bloco);
        layoutBloco->setContentsMargins(0, 0, 0, 0);
        layoutBloco->setSpacing(4);
        auto *selo = new QLabel(QString::fromUtf8("\xF0\x9F\xA9\xBA  PACIENTES"), bloco);
        selo->setStyleSheet(QString("color:%1; font-weight:800; letter-spacing:1px; font-size:12px; background:transparent;").arg(AppTheme::Azul));
        auto *titulo = new QLabel("Escolha uma opção", bloco);
        titulo->setStyleSheet("font-size:24px; font-weight:800; color:#0B2545; background:transparent;");
        layoutBloco->addWidget(selo);
        layoutBloco->addWidget(titulo);
        layoutPrincipal->addWidget(bloco);

        auto *layoutOpcoes = new QGridLayout();
        layoutOpcoes->setSpacing(22);

        auto *btnCadastrar = criarOpcao(this, QString::fromUtf8("\xE2\x9C\xA8"), "Cadastrar",
                                        "Registrar um novo paciente no sistema.", AppTheme::Verde, AppTheme::VerdeEscuro);
        auto *btnAtualizar = criarOpcao(this, QString::fromUtf8("\xE2\x9C\x8F\xEF\xB8\x8F"), "Atualizar cadastro",
                                        "Editar nome, telefone ou endereço de um paciente.", AppTheme::Azul, AppTheme::AzulEscuro);
        auto *btnExcluir = criarOpcao(this, QString::fromUtf8("\xF0\x9F\x97\x91\xEF\xB8\x8F"), "Excluir cadastro",
                                      "Remover definitivamente um paciente do sistema.", AppTheme::Perigo, AppTheme::PerigoEscuro);
        auto *btnConsultar = criarOpcao(this, QString::fromUtf8("\xF0\x9F\x94\x8D"), "Consultar cadastro",
                                        "Buscar pacientes por CPF ou nome.", AppTheme::Azul, AppTheme::AzulEscuro);

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

        auto *rodape = new QLabel("Developed by Kayke", this);
        rodape->setAlignment(Qt::AlignRight);
        rodape->setStyleSheet("color:#8492A6; font-weight:600; font-size:11px; background:transparent;");
        layoutPrincipal->addWidget(rodape);
}