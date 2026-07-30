#include "ViewInicio.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

ViewInicio::ViewInicio(QWidget *parent) : QWidget(parent)
{
    configurarInterface();
}

void ViewInicio::configurarInterface()
{
    auto *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(40, 40, 40, 40);
    layoutPrincipal->setSpacing(30);

    auto *titulo = new QLabel("Sistema de Transporte - Prefeitura de Piranga", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    layoutPrincipal->addWidget(titulo);
    layoutPrincipal->addStretch();

    auto *layoutCards = new QHBoxLayout();
    layoutCards->setSpacing(20);

    auto criarCard = [this](const QString &texto, const QString &cor)
    {
        auto *card = new QPushButton(texto, this);
        card->setMinimumSize(200, 150);
        card->setStyleSheet(QString(
                                "QPushButton { background-color: %1; color: white; font-size: 18px; "
                                "font-weight: bold; border-radius: 10px; }"
                                "QPushButton:hover { background-color: #34495e; }")
                                .arg(cor));
        return card;
    };

    auto *cardPacientes = criarCard("Pacientes", "#2980b9");
    auto *cardViagens = criarCard("Viagens", "#27ae60");
    auto *cardRelatorios = criarCard("Relatórios", "#8e44ad");

    connect(cardPacientes, &QPushButton::clicked, this, &ViewInicio::pacientesSelecionado);
    connect(cardViagens, &QPushButton::clicked, this, &ViewInicio::viagensSelecionado);
    connect(cardRelatorios, &QPushButton::clicked, this, &ViewInicio::relatoriosSelecionado);

    layoutCards->addWidget(cardPacientes);
    layoutCards->addWidget(cardViagens);
    layoutCards->addWidget(cardRelatorios);

    layoutPrincipal->addLayout(layoutCards);
    layoutPrincipal->addStretch();
}