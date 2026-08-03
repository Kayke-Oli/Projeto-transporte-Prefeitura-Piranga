#include "ViewInicio.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
ViewInicio::ViewInicio(QWidget *parent) : QWidget(parent) { configurarInterface(); }
void ViewInicio::configurarInterface()
{
    auto *principal = new QVBoxLayout(this);
    principal->setContentsMargins(64, 56, 64, 56);
    principal->setSpacing(18);
    auto *marca = new QLabel("PREFEITURA DE PIRANGA", this);
    marca->setAlignment(Qt::AlignCenter);
    marca->setStyleSheet("color:#2e6fbb;font-weight:700;letter-spacing:1px;");
    principal->addWidget(marca);
    auto *titulo = new QLabel("Logística de Saúde", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size:34px;font-weight:800;color:#102a43;");
    principal->addWidget(titulo);
    auto *sub = new QLabel("Tratamento Fora de Domicílio • organização segura de pacientes, transportes e relatórios", this);
    sub->setAlignment(Qt::AlignCenter);
    sub->setWordWrap(true);
    sub->setStyleSheet("color:#52606d;font-size:15px;");
    principal->addWidget(sub);
    principal->addSpacing(24);
    auto *grade = new QGridLayout();
    grade->setHorizontalSpacing(20);
    grade->setVerticalSpacing(20);
    auto criar = [this](const QString &t, const QString &d, const QString &cor)
    {auto*b=new QPushButton(t+"\n"+d,this);b->setMinimumSize(260,140);b->setCursor(Qt::PointingHandCursor);b->setStyleSheet(QString("QPushButton{background:%1;color:white;border:0;border-radius:14px;text-align:left;padding:20px;font-size:19px;font-weight:700;}QPushButton:hover{background:#102a43;}").arg(cor));return b; };
    auto *p = criar("Pacientes", "Cadastros e consultas", "#2e6fbb");
    auto *v = criar("Viagens", "Planejar e acompanhar", "#15803d");
    auto *r = criar("Relatórios", "Histórico e mapas", "#7c3aed");
    auto *c = criar("Cadastros auxiliares", "Motoristas, veículos e acompanhantes", "#d97706");
    connect(p, &QPushButton::clicked, this, &ViewInicio::pacientesSelecionado);
    connect(v, &QPushButton::clicked, this, &ViewInicio::viagensSelecionado);
    connect(r, &QPushButton::clicked, this, &ViewInicio::relatoriosSelecionado);
    connect(c, &QPushButton::clicked, this, &ViewInicio::cadastrosSelecionado);
    grade->addWidget(p, 0, 0);
    grade->addWidget(v, 0, 1);
    grade->addWidget(r, 1, 0);
    grade->addWidget(c, 1, 1);
    principal->addLayout(grade);
    principal->addStretch();
    auto *rodape = new QLabel("Sistema interno • Dados de saúde exigem atenção e conferência antes de salvar.", this);
    rodape->setAlignment(Qt::AlignCenter);
    rodape->setStyleSheet("color:#7b8794;");
    principal->addWidget(rodape);
}
