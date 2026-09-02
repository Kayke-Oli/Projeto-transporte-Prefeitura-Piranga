#include "ViewInicio.h"
#include "AppTheme.h"
#include "ClickableCard.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace
{
    ClickableCard *criarCartao(QWidget *pai, const QString &emoji, const QString &titulo,
                               const QString &descricao, const QString &corTopo, const QString &corTopoHover)
    {
        auto *botao = new ClickableCard(pai);
        botao->setMinimumSize(250, 156);

        // O conteúdo visual do cartão vem de um layout próprio (ícone,
        // título e descrição em labels), não do texto plano do botão.
        auto *layout = new QVBoxLayout(botao);
        layout->setContentsMargins(22, 20, 20, 18);
        layout->setSpacing(6);

        auto *lblEmoji = new QLabel(emoji, botao);
        lblEmoji->setStyleSheet("font-size:30px; background:transparent;");
        lblEmoji->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto *lblTitulo = new QLabel(titulo, botao);
        lblTitulo->setStyleSheet("font-size:17px; font-weight:800; color:#0B2545; background:transparent;");
        lblTitulo->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto *lblDescricao = new QLabel(descricao, botao);
        lblDescricao->setWordWrap(true);
        lblDescricao->setStyleSheet("font-size:12px; font-weight:600; color:#5A6B7A; background:transparent;");
        lblDescricao->setAttribute(Qt::WA_TransparentForMouseEvents);

        layout->addWidget(lblEmoji);
        layout->addWidget(lblTitulo);
        layout->addWidget(lblDescricao);
        layout->addStretch();

        botao->setStyleSheet(QString(R"(
            ClickableCard {
                background: #ffffff;
                border: 1px solid #DCE4EC;
                border-top: 4px solid %1;
                border-radius: 16px;
            }
            ClickableCard:hover {
                border-top: 4px solid %2;
                background: #FAFCFE;
            }
        )")
                                 .arg(corTopo, corTopoHover));

        return botao;
    }
}

ViewInicio::ViewInicio(QWidget *parent) : QWidget(parent) { configurarInterface(); }

void ViewInicio::configurarInterface()
{
    auto *principal = new QVBoxLayout(this);
    principal->setContentsMargins(56, 40, 56, 32);
    principal->setSpacing(28);

    // ---------------------------------------------------------------
    // Faixa "hero": gradiente com as cores oficiais da Prefeitura de
    // Piranga (verde -> azul) e a logo em destaque.
    // ---------------------------------------------------------------
    auto *hero = new QWidget(this);
    hero->setObjectName("heroInicio");
    hero->setMinimumHeight(190);
    hero->setStyleSheet(QString(R"(
        QWidget#heroInicio {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 %1, stop:0.55 %1, stop:0.55 %2, stop:1 %2);
            border-radius: 20px;
        }
    )")
                            .arg(AppTheme::Verde, AppTheme::Azul));

    auto *layoutHero = new QHBoxLayout(hero);
    layoutHero->setContentsMargins(40, 28, 40, 28);
    layoutHero->setSpacing(28);

    auto *logoHero = new QLabel(hero);
    logoHero->setPixmap(AppTheme::logoPixmap(96));
    logoHero->setFixedSize(192, 96);
    layoutHero->addWidget(logoHero, 0, Qt::AlignVCenter);

    auto *textoHero = new QWidget(hero);
    textoHero->setStyleSheet("background: transparent;");
    auto *layoutTextoHero = new QVBoxLayout(textoHero);
    layoutTextoHero->setContentsMargins(0, 0, 0, 0);
    layoutTextoHero->setSpacing(6);

    auto *marca = new QLabel("PREFEITURA DE PIRANGA  •  UM NOVO TEMPO", textoHero);
    marca->setStyleSheet("color:rgba(255,255,255,0.85); font-weight:800; letter-spacing:1.5px; font-size:11px; background:transparent;");
    layoutTextoHero->addWidget(marca);

    auto *titulo = new QLabel("Logística de Saúde", textoHero);
    titulo->setStyleSheet("font-size:32px; font-weight:800; color:#ffffff; background:transparent;");
    layoutTextoHero->addWidget(titulo);

    auto *sub = new QLabel("Tratamento Fora de Domicílio — organização segura de pacientes, transportes e relatórios", textoHero);
    sub->setWordWrap(true);
    sub->setStyleSheet("color:rgba(255,255,255,0.92); font-size:14px; font-weight:600; background:transparent;");
    layoutTextoHero->addWidget(sub);

    layoutTextoHero->addStretch();
    layoutHero->addWidget(textoHero, 1);
    principal->addWidget(hero);

    // ---------------------------------------------------------------
    // Cartões de navegação
    // ---------------------------------------------------------------
    auto *grade = new QGridLayout();
    grade->setHorizontalSpacing(22);
    grade->setVerticalSpacing(22);

    auto *p = criarCartao(this, QString::fromUtf8("\xF0\x9F\xA9\xBA"), "Pacientes",
                          "Cadastros, buscas e atualização de dados dos pacientes atendidos.",
                          AppTheme::Azul, AppTheme::AzulEscuro);
    auto *v = criarCartao(this, QString::fromUtf8("\xF0\x9F\x9A\x90"), "Viagens",
                          "Planeje viagens, monte a lista de passageiros e acompanhe cada saída.",
                          AppTheme::Verde, AppTheme::VerdeEscuro);
    auto *r = criarCartao(this, QString::fromUtf8("\xF0\x9F\x93\x8A"), "Relatórios",
                          "Histórico por paciente, volume de passageiros e mapa diário de viagens.",
                          AppTheme::Azul, AppTheme::AzulEscuro);
    auto *c = criarCartao(this, QString::fromUtf8("\xF0\x9F\x93\x87"), "Cadastros auxiliares",
                          "Motoristas, veículos, acompanhantes e auxiliares de viagem.",
                          AppTheme::Verde, AppTheme::VerdeEscuro);

    connect(p, &ClickableCard::clicked, this, &ViewInicio::pacientesSelecionado);
    connect(v, &ClickableCard::clicked, this, &ViewInicio::viagensSelecionado);
    connect(r, &ClickableCard::clicked, this, &ViewInicio::relatoriosSelecionado);
    connect(c, &ClickableCard::clicked, this, &ViewInicio::cadastrosSelecionado);

    grade->addWidget(p, 0, 0);
    grade->addWidget(v, 0, 1);
    grade->addWidget(r, 1, 0);
    grade->addWidget(c, 1, 1);
    principal->addLayout(grade);
    principal->addStretch();

    // ---------------------------------------------------------------
    // Rodapé
    // ---------------------------------------------------------------
    auto *rodape = new QLabel(
        "Sistema interno • Dados de saúde exigem atenção e conferência antes de salvar.  ·  Developed by Kayke",
        this);
    rodape->setAlignment(Qt::AlignCenter);
    rodape->setStyleSheet("color:#8492A6; font-weight:600; background:transparent;");
    principal->addWidget(rodape);
}