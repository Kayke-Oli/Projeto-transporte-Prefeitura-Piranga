/********************************************************************************
** Form generated from reading UI file 'ViewPacientes.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWPACIENTES_H
#define UI_VIEWPACIENTES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewPacientes
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *layoutTopo;
    QPushButton *btnVoltar;
    QLabel *labelTitulo;
    QSpacerItem *spacerTopo;
    QLabel *labelInstrucao;
    QGroupBox *grupoDados;
    QFormLayout *formDados;
    QLabel *labelNome;
    QLineEdit *txtNome;
    QLabel *labelCpf;
    QLineEdit *txtCpf;
    QLabel *labelTelefone;
    QLineEdit *txtTelefone;
    QLabel *labelEndereco;
    QLineEdit *txtEndereco;
    QGroupBox *grupoBuscaNome;
    QHBoxLayout *layoutBuscaNome;
    QLineEdit *txtBuscaNome;
    QTableWidget *tabelaResultados;
    QLabel *labelMensagem;
    QHBoxLayout *layoutAcoes;
    QPushButton *btnLimpar;
    QSpacerItem *spacerAcoes;
    QPushButton *btnSalvar;
    QPushButton *btnAtualizar;
    QPushButton *btnDeletar;
    QPushButton *btnConsultar;

    void setupUi(QWidget *ViewPacientes)
    {
        if (ViewPacientes->objectName().isEmpty())
            ViewPacientes->setObjectName("ViewPacientes");
        ViewPacientes->resize(760, 620);
        ViewPacientes->setMinimumSize(QSize(680, 560));
        ViewPacientes->setStyleSheet(QString::fromUtf8("QWidget#ViewPacientes {\n"
"    background: #f7f9fc;\n"
"}\n"
"QGroupBox {\n"
"    background: #ffffff;\n"
"    border: 1px solid #d0d5dd;\n"
"    border-radius: 8px;\n"
"    margin-top: 12px;\n"
"    padding: 14px 12px 10px 12px;\n"
"    font-weight: 600;\n"
"    color: #344054;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 12px;\n"
"    padding: 0 4px;\n"
"}\n"
"QLineEdit {\n"
"    background: #ffffff;\n"
"    border: 1px solid #98a2b3;\n"
"    border-radius: 5px;\n"
"    padding: 7px 8px;\n"
"    min-height: 20px;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 2px solid #2e6fbb;\n"
"}\n"
"QLineEdit:disabled {\n"
"    background: #eaecf0;\n"
"    color: #667085;\n"
"}\n"
"QTableWidget {\n"
"    background: #ffffff;\n"
"    border: 1px solid #d0d5dd;\n"
"    border-radius: 6px;\n"
"    gridline-color: #eaecf0;\n"
"}\n"
"QHeaderView::section {\n"
"    background: #eef3f8;\n"
"    color: #344054;\n"
"    border: 0;\n"
"    border-bottom: 1px solid #d0d5dd;\n"
"    padding: 7px;\n"
""
                        "    font-weight: 600;\n"
"}\n"
"QPushButton {\n"
"    min-height: 32px;\n"
"    border-radius: 5px;\n"
"    padding: 0 14px;\n"
"    font-weight: 600;\n"
"}\n"
"QPushButton:hover { background: #dbeafe; }\n"
"QPushButton:disabled { background: #eaecf0; color: #98a2b3; }\n"
"QPushButton#btnSalvar, QPushButton#btnAtualizar, QPushButton#btnConsultar {\n"
"    background: #1d4f91;\n"
"    color: white;\n"
"    border: 0;\n"
"}\n"
"QPushButton#btnSalvar:hover, QPushButton#btnAtualizar:hover, QPushButton#btnConsultar:hover {\n"
"    background: #163e72;\n"
"}\n"
"QPushButton#btnDeletar {\n"
"    background: #b42318;\n"
"    color: white;\n"
"    border: 0;\n"
"}\n"
"QPushButton#btnDeletar:hover { background: #8a1c1c; }\n"
"QPushButton#btnLimpar { background: #ffffff; color: #344054; border: 1px solid #98a2b3; }\n"
"QPushButton#btnVoltar { background: transparent; color: #1d4f91; border: 0; }"));
        verticalLayout = new QVBoxLayout(ViewPacientes);
        verticalLayout->setSpacing(14);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(32, 28, 32, 28);
        layoutTopo = new QHBoxLayout();
        layoutTopo->setObjectName("layoutTopo");
        btnVoltar = new QPushButton(ViewPacientes);
        btnVoltar->setObjectName("btnVoltar");
        btnVoltar->setMaximumSize(QSize(110, 16777215));

        layoutTopo->addWidget(btnVoltar);

        labelTitulo = new QLabel(ViewPacientes);
        labelTitulo->setObjectName("labelTitulo");
        labelTitulo->setStyleSheet(QString::fromUtf8("font-size: 22px; font-weight: 700; color: #1d2939;"));
        labelTitulo->setAlignment(Qt::AlignCenter);

        layoutTopo->addWidget(labelTitulo);

        spacerTopo = new QSpacerItem(110, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutTopo->addItem(spacerTopo);


        verticalLayout->addLayout(layoutTopo);

        labelInstrucao = new QLabel(ViewPacientes);
        labelInstrucao->setObjectName("labelInstrucao");
        labelInstrucao->setStyleSheet(QString::fromUtf8("color: #475467; font-size: 13px;"));
        labelInstrucao->setAlignment(Qt::AlignCenter);
        labelInstrucao->setWordWrap(true);

        verticalLayout->addWidget(labelInstrucao);

        grupoDados = new QGroupBox(ViewPacientes);
        grupoDados->setObjectName("grupoDados");
        formDados = new QFormLayout(grupoDados);
        formDados->setObjectName("formDados");
        formDados->setHorizontalSpacing(14);
        formDados->setVerticalSpacing(10);
        labelNome = new QLabel(grupoDados);
        labelNome->setObjectName("labelNome");

        formDados->setWidget(0, QFormLayout::ItemRole::LabelRole, labelNome);

        txtNome = new QLineEdit(grupoDados);
        txtNome->setObjectName("txtNome");
        txtNome->setMaxLength(150);

        formDados->setWidget(0, QFormLayout::ItemRole::FieldRole, txtNome);

        labelCpf = new QLabel(grupoDados);
        labelCpf->setObjectName("labelCpf");

        formDados->setWidget(1, QFormLayout::ItemRole::LabelRole, labelCpf);

        txtCpf = new QLineEdit(grupoDados);
        txtCpf->setObjectName("txtCpf");
        txtCpf->setMaxLength(14);

        formDados->setWidget(1, QFormLayout::ItemRole::FieldRole, txtCpf);

        labelTelefone = new QLabel(grupoDados);
        labelTelefone->setObjectName("labelTelefone");

        formDados->setWidget(2, QFormLayout::ItemRole::LabelRole, labelTelefone);

        txtTelefone = new QLineEdit(grupoDados);
        txtTelefone->setObjectName("txtTelefone");
        txtTelefone->setMaxLength(15);

        formDados->setWidget(2, QFormLayout::ItemRole::FieldRole, txtTelefone);

        labelEndereco = new QLabel(grupoDados);
        labelEndereco->setObjectName("labelEndereco");

        formDados->setWidget(3, QFormLayout::ItemRole::LabelRole, labelEndereco);

        txtEndereco = new QLineEdit(grupoDados);
        txtEndereco->setObjectName("txtEndereco");
        txtEndereco->setMaxLength(255);

        formDados->setWidget(3, QFormLayout::ItemRole::FieldRole, txtEndereco);


        verticalLayout->addWidget(grupoDados);

        grupoBuscaNome = new QGroupBox(ViewPacientes);
        grupoBuscaNome->setObjectName("grupoBuscaNome");
        layoutBuscaNome = new QHBoxLayout(grupoBuscaNome);
        layoutBuscaNome->setObjectName("layoutBuscaNome");
        txtBuscaNome = new QLineEdit(grupoBuscaNome);
        txtBuscaNome->setObjectName("txtBuscaNome");
        txtBuscaNome->setMaxLength(150);

        layoutBuscaNome->addWidget(txtBuscaNome);


        verticalLayout->addWidget(grupoBuscaNome);

        tabelaResultados = new QTableWidget(ViewPacientes);
        tabelaResultados->setObjectName("tabelaResultados");
        tabelaResultados->setMinimumSize(QSize(0, 145));
        tabelaResultados->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tabelaResultados->setSelectionMode(QAbstractItemView::SingleSelection);
        tabelaResultados->setSelectionBehavior(QAbstractItemView::SelectRows);
        tabelaResultados->setShowGrid(false);
        tabelaResultados->setAlternatingRowColors(true);
        tabelaResultados->setRowCount(0);
        tabelaResultados->setColumnCount(3);

        verticalLayout->addWidget(tabelaResultados);

        labelMensagem = new QLabel(ViewPacientes);
        labelMensagem->setObjectName("labelMensagem");
        labelMensagem->setMinimumSize(QSize(0, 22));
        labelMensagem->setWordWrap(true);

        verticalLayout->addWidget(labelMensagem);

        layoutAcoes = new QHBoxLayout();
        layoutAcoes->setObjectName("layoutAcoes");
        btnLimpar = new QPushButton(ViewPacientes);
        btnLimpar->setObjectName("btnLimpar");

        layoutAcoes->addWidget(btnLimpar);

        spacerAcoes = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutAcoes->addItem(spacerAcoes);

        btnSalvar = new QPushButton(ViewPacientes);
        btnSalvar->setObjectName("btnSalvar");

        layoutAcoes->addWidget(btnSalvar);

        btnAtualizar = new QPushButton(ViewPacientes);
        btnAtualizar->setObjectName("btnAtualizar");

        layoutAcoes->addWidget(btnAtualizar);

        btnDeletar = new QPushButton(ViewPacientes);
        btnDeletar->setObjectName("btnDeletar");

        layoutAcoes->addWidget(btnDeletar);

        btnConsultar = new QPushButton(ViewPacientes);
        btnConsultar->setObjectName("btnConsultar");

        layoutAcoes->addWidget(btnConsultar);


        verticalLayout->addLayout(layoutAcoes);


        retranslateUi(ViewPacientes);

        QMetaObject::connectSlotsByName(ViewPacientes);
    } // setupUi

    void retranslateUi(QWidget *ViewPacientes)
    {
        ViewPacientes->setWindowTitle(QCoreApplication::translate("ViewPacientes", "Gerenciamento de pacientes", nullptr));
        btnVoltar->setText(QCoreApplication::translate("ViewPacientes", "< Voltar", nullptr));
        labelTitulo->setText(QCoreApplication::translate("ViewPacientes", "Pacientes", nullptr));
        labelInstrucao->setText(QString());
        grupoDados->setTitle(QCoreApplication::translate("ViewPacientes", "Dados do paciente", nullptr));
        labelNome->setText(QCoreApplication::translate("ViewPacientes", "Nome completo *", nullptr));
        txtNome->setPlaceholderText(QCoreApplication::translate("ViewPacientes", "Nome completo do paciente", nullptr));
        labelCpf->setText(QCoreApplication::translate("ViewPacientes", "CPF *", nullptr));
        txtCpf->setPlaceholderText(QCoreApplication::translate("ViewPacientes", "000.000.000-00", nullptr));
        labelTelefone->setText(QCoreApplication::translate("ViewPacientes", "Telefone", nullptr));
        txtTelefone->setPlaceholderText(QCoreApplication::translate("ViewPacientes", "(00) 0000-0000 ou (00) 00000-0000", nullptr));
        labelEndereco->setText(QCoreApplication::translate("ViewPacientes", "Endere\303\247o", nullptr));
        txtEndereco->setPlaceholderText(QCoreApplication::translate("ViewPacientes", "Rua, n\303\272mero, bairro e refer\303\252ncia", nullptr));
        grupoBuscaNome->setTitle(QCoreApplication::translate("ViewPacientes", "Busca por nome", nullptr));
        txtBuscaNome->setPlaceholderText(QCoreApplication::translate("ViewPacientes", "Digite pelo menos tr\303\252s letras do nome", nullptr));
        labelMensagem->setText(QString());
        btnLimpar->setText(QCoreApplication::translate("ViewPacientes", "Limpar", nullptr));
        btnSalvar->setText(QCoreApplication::translate("ViewPacientes", "Salvar cadastro", nullptr));
        btnAtualizar->setText(QCoreApplication::translate("ViewPacientes", "Salvar altera\303\247\303\265es", nullptr));
        btnDeletar->setText(QCoreApplication::translate("ViewPacientes", "Excluir cadastro", nullptr));
        btnConsultar->setText(QCoreApplication::translate("ViewPacientes", "Buscar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewPacientes: public Ui_ViewPacientes {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWPACIENTES_H
