/********************************************************************************
** Form generated from reading UI file 'ViewViagens.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWVIAGENS_H
#define UI_VIEWVIAGENS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewViagens
{
public:
    QVBoxLayout *v;
    QHBoxLayout *top;
    QPushButton *btnVoltar;
    QLabel *title;
    QGroupBox *gDados;
    QFormLayout *f1;
    QLabel *lData;
    QDateEdit *dtViagem;
    QLabel *lDest;
    QLineEdit *txtDestino;
    QLabel *lVeic;
    QComboBox *cmbVeiculo;
    QLabel *lMot;
    QComboBox *cmbMotorista;
    QLabel *lAux;
    QComboBox *cmbAuxiliar;
    QGroupBox *gPass;
    QGridLayout *grid;
    QLineEdit *txtCpfPaciente;
    QPushButton *btnBuscarPaciente;
    QLabel *lblPaciente;
    QLabel *lAcompanhante;
    QLineEdit *txtNomeAcompanhante;
    QLineEdit *txtCpfAcompanhante;
    QLineEdit *txtTelefoneAcompanhante;
    QPushButton *btnVerificarAcompanhante;
    QLabel *lblAcompanhante;
    QPushButton *btnAdicionar;
    QPushButton *btnRemover;
    QTableWidget *tblPassageiros;
    QLabel *lblMensagem;
    QHBoxLayout *a;
    QPushButton *btnLimpar;
    QPushButton *btnSalvar;
    QPushButton *btnExcluir;
    QPushButton *btnListar;
    QPushButton *btnExportarPlanilha;
    QPushButton *btnGerarPdf;
    QTableWidget *tblViagens;

    void setupUi(QWidget *ViewViagens)
    {
        if (ViewViagens->objectName().isEmpty())
            ViewViagens->setObjectName("ViewViagens");
        v = new QVBoxLayout(ViewViagens);
        v->setObjectName("v");
        v->setContentsMargins(32, 24, 32, -1);
        top = new QHBoxLayout();
        top->setObjectName("top");
        btnVoltar = new QPushButton(ViewViagens);
        btnVoltar->setObjectName("btnVoltar");

        top->addWidget(btnVoltar);

        title = new QLabel(ViewViagens);
        title->setObjectName("title");
        title->setStyleSheet(QString::fromUtf8("font-size:24px;font-weight:700;"));

        top->addWidget(title);


        v->addLayout(top);

        gDados = new QGroupBox(ViewViagens);
        gDados->setObjectName("gDados");
        f1 = new QFormLayout(gDados);
        f1->setObjectName("f1");
        lData = new QLabel(gDados);
        lData->setObjectName("lData");

        f1->setWidget(0, QFormLayout::ItemRole::LabelRole, lData);

        dtViagem = new QDateEdit(gDados);
        dtViagem->setObjectName("dtViagem");
        dtViagem->setCalendarPopup(true);

        f1->setWidget(0, QFormLayout::ItemRole::FieldRole, dtViagem);

        lDest = new QLabel(gDados);
        lDest->setObjectName("lDest");

        f1->setWidget(1, QFormLayout::ItemRole::LabelRole, lDest);

        txtDestino = new QLineEdit(gDados);
        txtDestino->setObjectName("txtDestino");
        txtDestino->setMaxLength(100);

        f1->setWidget(1, QFormLayout::ItemRole::FieldRole, txtDestino);

        lVeic = new QLabel(gDados);
        lVeic->setObjectName("lVeic");

        f1->setWidget(2, QFormLayout::ItemRole::LabelRole, lVeic);

        cmbVeiculo = new QComboBox(gDados);
        cmbVeiculo->setObjectName("cmbVeiculo");

        f1->setWidget(2, QFormLayout::ItemRole::FieldRole, cmbVeiculo);

        lMot = new QLabel(gDados);
        lMot->setObjectName("lMot");

        f1->setWidget(3, QFormLayout::ItemRole::LabelRole, lMot);

        cmbMotorista = new QComboBox(gDados);
        cmbMotorista->setObjectName("cmbMotorista");

        f1->setWidget(3, QFormLayout::ItemRole::FieldRole, cmbMotorista);

        lAux = new QLabel(gDados);
        lAux->setObjectName("lAux");

        f1->setWidget(4, QFormLayout::ItemRole::LabelRole, lAux);

        cmbAuxiliar = new QComboBox(gDados);
        cmbAuxiliar->setObjectName("cmbAuxiliar");

        f1->setWidget(4, QFormLayout::ItemRole::FieldRole, cmbAuxiliar);


        v->addWidget(gDados);

        gPass = new QGroupBox(ViewViagens);
        gPass->setObjectName("gPass");
        grid = new QGridLayout(gPass);
        grid->setObjectName("grid");
        txtCpfPaciente = new QLineEdit(gPass);
        txtCpfPaciente->setObjectName("txtCpfPaciente");

        grid->addWidget(txtCpfPaciente, 0, 0, 1, 1);

        btnBuscarPaciente = new QPushButton(gPass);
        btnBuscarPaciente->setObjectName("btnBuscarPaciente");

        grid->addWidget(btnBuscarPaciente, 0, 1, 1, 1);

        lblPaciente = new QLabel(gPass);
        lblPaciente->setObjectName("lblPaciente");

        grid->addWidget(lblPaciente, 0, 2, 1, 3);

        lAcompanhante = new QLabel(gPass);
        lAcompanhante->setObjectName("lAcompanhante");

        grid->addWidget(lAcompanhante, 1, 0, 1, 1);

        txtNomeAcompanhante = new QLineEdit(gPass);
        txtNomeAcompanhante->setObjectName("txtNomeAcompanhante");
        txtNomeAcompanhante->setMaxLength(150);

        grid->addWidget(txtNomeAcompanhante, 1, 1, 1, 1);

        txtCpfAcompanhante = new QLineEdit(gPass);
        txtCpfAcompanhante->setObjectName("txtCpfAcompanhante");
        txtCpfAcompanhante->setMaxLength(14);

        grid->addWidget(txtCpfAcompanhante, 1, 2, 1, 1);

        txtTelefoneAcompanhante = new QLineEdit(gPass);
        txtTelefoneAcompanhante->setObjectName("txtTelefoneAcompanhante");
        txtTelefoneAcompanhante->setMaxLength(15);

        grid->addWidget(txtTelefoneAcompanhante, 1, 3, 1, 1);

        btnVerificarAcompanhante = new QPushButton(gPass);
        btnVerificarAcompanhante->setObjectName("btnVerificarAcompanhante");

        grid->addWidget(btnVerificarAcompanhante, 1, 4, 1, 1);

        lblAcompanhante = new QLabel(gPass);
        lblAcompanhante->setObjectName("lblAcompanhante");

        grid->addWidget(lblAcompanhante, 2, 1, 1, 4);

        btnAdicionar = new QPushButton(gPass);
        btnAdicionar->setObjectName("btnAdicionar");

        grid->addWidget(btnAdicionar, 3, 0, 1, 1);

        btnRemover = new QPushButton(gPass);
        btnRemover->setObjectName("btnRemover");

        grid->addWidget(btnRemover, 3, 1, 1, 1);

        tblPassageiros = new QTableWidget(gPass);
        tblPassageiros->setObjectName("tblPassageiros");
        tblPassageiros->setColumnCount(3);
        tblPassageiros->setRowCount(0);

        grid->addWidget(tblPassageiros, 4, 0, 1, 5);


        v->addWidget(gPass);

        lblMensagem = new QLabel(ViewViagens);
        lblMensagem->setObjectName("lblMensagem");

        v->addWidget(lblMensagem);

        a = new QHBoxLayout();
        a->setObjectName("a");
        btnLimpar = new QPushButton(ViewViagens);
        btnLimpar->setObjectName("btnLimpar");

        a->addWidget(btnLimpar);

        btnSalvar = new QPushButton(ViewViagens);
        btnSalvar->setObjectName("btnSalvar");

        a->addWidget(btnSalvar);

        btnExcluir = new QPushButton(ViewViagens);
        btnExcluir->setObjectName("btnExcluir");

        a->addWidget(btnExcluir);

        btnListar = new QPushButton(ViewViagens);
        btnListar->setObjectName("btnListar");

        a->addWidget(btnListar);

        btnExportarPlanilha = new QPushButton(ViewViagens);
        btnExportarPlanilha->setObjectName("btnExportarPlanilha");
        btnExportarPlanilha->setVisible(false);

        a->addWidget(btnExportarPlanilha);

        btnGerarPdf = new QPushButton(ViewViagens);
        btnGerarPdf->setObjectName("btnGerarPdf");
        btnGerarPdf->setVisible(false);

        a->addWidget(btnGerarPdf);


        v->addLayout(a);

        tblViagens = new QTableWidget(ViewViagens);
        tblViagens->setObjectName("tblViagens");
        tblViagens->setColumnCount(4);
        tblViagens->setRowCount(0);

        v->addWidget(tblViagens);


        retranslateUi(ViewViagens);

        QMetaObject::connectSlotsByName(ViewViagens);
    } // setupUi

    void retranslateUi(QWidget *ViewViagens)
    {
        btnVoltar->setText(QCoreApplication::translate("ViewViagens", "< Voltar", nullptr));
        title->setText(QCoreApplication::translate("ViewViagens", "Planejamento de viagens TFD", nullptr));
        gDados->setTitle(QCoreApplication::translate("ViewViagens", "Dados da viagem", nullptr));
        lData->setText(QCoreApplication::translate("ViewViagens", "Data", nullptr));
        dtViagem->setDisplayFormat(QCoreApplication::translate("ViewViagens", "dd-MM-yyyy", nullptr));
        lDest->setText(QCoreApplication::translate("ViewViagens", "Destino", nullptr));
        lVeic->setText(QCoreApplication::translate("ViewViagens", "Ve\303\255culo", nullptr));
        lMot->setText(QCoreApplication::translate("ViewViagens", "Motorista", nullptr));
        lAux->setText(QCoreApplication::translate("ViewViagens", "Auxiliar de viagem (opcional)", nullptr));
        gPass->setTitle(QCoreApplication::translate("ViewViagens", "Pacientes e acompanhantes", nullptr));
        txtCpfPaciente->setPlaceholderText(QCoreApplication::translate("ViewViagens", "CPF do paciente", nullptr));
        btnBuscarPaciente->setText(QCoreApplication::translate("ViewViagens", "Buscar paciente", nullptr));
        lAcompanhante->setText(QCoreApplication::translate("ViewViagens", "Acompanhante (opcional)", nullptr));
        txtNomeAcompanhante->setPlaceholderText(QCoreApplication::translate("ViewViagens", "Nome", nullptr));
        txtCpfAcompanhante->setPlaceholderText(QCoreApplication::translate("ViewViagens", "CPF", nullptr));
        txtTelefoneAcompanhante->setPlaceholderText(QCoreApplication::translate("ViewViagens", "Telefone com DDD", nullptr));
        btnVerificarAcompanhante->setText(QCoreApplication::translate("ViewViagens", "Verificar acompanhante", nullptr));
        btnAdicionar->setText(QCoreApplication::translate("ViewViagens", "Adicionar \303\240 viagem", nullptr));
        btnRemover->setText(QCoreApplication::translate("ViewViagens", "Remover selecionado", nullptr));
        btnLimpar->setText(QCoreApplication::translate("ViewViagens", "Nova viagem", nullptr));
        btnSalvar->setText(QCoreApplication::translate("ViewViagens", "Salvar viagem", nullptr));
        btnExcluir->setText(QCoreApplication::translate("ViewViagens", "Excluir viagem", nullptr));
        btnListar->setText(QCoreApplication::translate("ViewViagens", "Listar na data", nullptr));
        btnExportarPlanilha->setText(QCoreApplication::translate("ViewViagens", "Exportar planilha", nullptr));
        btnGerarPdf->setText(QCoreApplication::translate("ViewViagens", "Gerar PDF", nullptr));
        (void)ViewViagens;
    } // retranslateUi

};

namespace Ui {
    class ViewViagens: public Ui_ViewViagens {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWVIAGENS_H
