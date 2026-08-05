/********************************************************************************
** Form generated from reading UI file 'ViewRelatorios.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWRELATORIOS_H
#define UI_VIEWRELATORIOS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewRelatorios
{
public:
    QVBoxLayout *v;
    QHBoxLayout *t;
    QPushButton *btnVoltar;
    QLabel *title;
    QTabWidget *tabs;
    QWidget *tabRelatorioViagem;
    QVBoxLayout *vr;
    QLabel *lblExplicacaoRelatorio;
    QFormLayout *fr;
    QLabel *lDataRelatorio;
    QDateEdit *dtRelatorioViagem;
    QLabel *lDestinoRelatorio;
    QComboBox *cmbDestinoRelatorio;
    QHBoxLayout *ar;
    QPushButton *btnExportarPlanilhaViagem;
    QPushButton *btnGerarPdfViagem;
    QSpacerItem *spacerRelatorio;
    QWidget *tabH;
    QVBoxLayout *vh;
    QHBoxLayout *bh;
    QLineEdit *txtCpfHistorico;
    QPushButton *btnHistorico;
    QTableWidget *tblHistorico;
    QWidget *tabV;
    QVBoxLayout *vv;
    QHBoxLayout *bv;
    QDateEdit *dtInicio;
    QDateEdit *dtFim;
    QPushButton *btnVolume;
    QLabel *l1;
    QLabel *lblPacientes;
    QLabel *l2;
    QLabel *lblAcompanhantes;
    QLabel *l3;
    QLabel *lblPessoas;
    QWidget *tabM;
    QVBoxLayout *vm;
    QHBoxLayout *bm;
    QDateEdit *dtMapa;
    QPushButton *btnMapa;
    QPlainTextEdit *txtMapa;
    QLabel *lblMensagem;

    void setupUi(QWidget *ViewRelatorios)
    {
        if (ViewRelatorios->objectName().isEmpty())
            ViewRelatorios->setObjectName("ViewRelatorios");
        v = new QVBoxLayout(ViewRelatorios);
        v->setObjectName("v");
        v->setContentsMargins(32, 24, 32, -1);
        t = new QHBoxLayout();
        t->setObjectName("t");
        btnVoltar = new QPushButton(ViewRelatorios);
        btnVoltar->setObjectName("btnVoltar");

        t->addWidget(btnVoltar);

        title = new QLabel(ViewRelatorios);
        title->setObjectName("title");
        title->setStyleSheet(QString::fromUtf8("font-size:24px;font-weight:700;"));

        t->addWidget(title);


        v->addLayout(t);

        tabs = new QTabWidget(ViewRelatorios);
        tabs->setObjectName("tabs");
        tabRelatorioViagem = new QWidget();
        tabRelatorioViagem->setObjectName("tabRelatorioViagem");
        vr = new QVBoxLayout(tabRelatorioViagem);
        vr->setObjectName("vr");
        lblExplicacaoRelatorio = new QLabel(tabRelatorioViagem);
        lblExplicacaoRelatorio->setObjectName("lblExplicacaoRelatorio");
        lblExplicacaoRelatorio->setWordWrap(true);

        vr->addWidget(lblExplicacaoRelatorio);

        fr = new QFormLayout();
        fr->setObjectName("fr");
        lDataRelatorio = new QLabel(tabRelatorioViagem);
        lDataRelatorio->setObjectName("lDataRelatorio");

        fr->setWidget(0, QFormLayout::ItemRole::LabelRole, lDataRelatorio);

        dtRelatorioViagem = new QDateEdit(tabRelatorioViagem);
        dtRelatorioViagem->setObjectName("dtRelatorioViagem");
        dtRelatorioViagem->setCalendarPopup(true);

        fr->setWidget(0, QFormLayout::ItemRole::FieldRole, dtRelatorioViagem);

        lDestinoRelatorio = new QLabel(tabRelatorioViagem);
        lDestinoRelatorio->setObjectName("lDestinoRelatorio");

        fr->setWidget(1, QFormLayout::ItemRole::LabelRole, lDestinoRelatorio);

        cmbDestinoRelatorio = new QComboBox(tabRelatorioViagem);
        cmbDestinoRelatorio->setObjectName("cmbDestinoRelatorio");

        fr->setWidget(1, QFormLayout::ItemRole::FieldRole, cmbDestinoRelatorio);


        vr->addLayout(fr);

        ar = new QHBoxLayout();
        ar->setObjectName("ar");
        btnExportarPlanilhaViagem = new QPushButton(tabRelatorioViagem);
        btnExportarPlanilhaViagem->setObjectName("btnExportarPlanilhaViagem");

        ar->addWidget(btnExportarPlanilhaViagem);

        btnGerarPdfViagem = new QPushButton(tabRelatorioViagem);
        btnGerarPdfViagem->setObjectName("btnGerarPdfViagem");

        ar->addWidget(btnGerarPdfViagem);


        vr->addLayout(ar);

        spacerRelatorio = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vr->addItem(spacerRelatorio);

        tabs->addTab(tabRelatorioViagem, QString());
        tabH = new QWidget();
        tabH->setObjectName("tabH");
        vh = new QVBoxLayout(tabH);
        vh->setObjectName("vh");
        bh = new QHBoxLayout();
        bh->setObjectName("bh");
        txtCpfHistorico = new QLineEdit(tabH);
        txtCpfHistorico->setObjectName("txtCpfHistorico");

        bh->addWidget(txtCpfHistorico);

        btnHistorico = new QPushButton(tabH);
        btnHistorico->setObjectName("btnHistorico");

        bh->addWidget(btnHistorico);


        vh->addLayout(bh);

        tblHistorico = new QTableWidget(tabH);
        tblHistorico->setObjectName("tblHistorico");
        tblHistorico->setColumnCount(4);
        tblHistorico->setRowCount(0);

        vh->addWidget(tblHistorico);

        tabs->addTab(tabH, QString());
        tabV = new QWidget();
        tabV->setObjectName("tabV");
        vv = new QVBoxLayout(tabV);
        vv->setObjectName("vv");
        bv = new QHBoxLayout();
        bv->setObjectName("bv");
        dtInicio = new QDateEdit(tabV);
        dtInicio->setObjectName("dtInicio");
        dtInicio->setCalendarPopup(true);

        bv->addWidget(dtInicio);

        dtFim = new QDateEdit(tabV);
        dtFim->setObjectName("dtFim");
        dtFim->setCalendarPopup(true);

        bv->addWidget(dtFim);

        btnVolume = new QPushButton(tabV);
        btnVolume->setObjectName("btnVolume");

        bv->addWidget(btnVolume);


        vv->addLayout(bv);

        l1 = new QLabel(tabV);
        l1->setObjectName("l1");

        vv->addWidget(l1);

        lblPacientes = new QLabel(tabV);
        lblPacientes->setObjectName("lblPacientes");
        lblPacientes->setStyleSheet(QString::fromUtf8("font-size:28px;font-weight:700;"));

        vv->addWidget(lblPacientes);

        l2 = new QLabel(tabV);
        l2->setObjectName("l2");

        vv->addWidget(l2);

        lblAcompanhantes = new QLabel(tabV);
        lblAcompanhantes->setObjectName("lblAcompanhantes");
        lblAcompanhantes->setStyleSheet(QString::fromUtf8("font-size:28px;font-weight:700;"));

        vv->addWidget(lblAcompanhantes);

        l3 = new QLabel(tabV);
        l3->setObjectName("l3");

        vv->addWidget(l3);

        lblPessoas = new QLabel(tabV);
        lblPessoas->setObjectName("lblPessoas");
        lblPessoas->setStyleSheet(QString::fromUtf8("font-size:28px;font-weight:700;"));

        vv->addWidget(lblPessoas);

        tabs->addTab(tabV, QString());
        tabM = new QWidget();
        tabM->setObjectName("tabM");
        vm = new QVBoxLayout(tabM);
        vm->setObjectName("vm");
        bm = new QHBoxLayout();
        bm->setObjectName("bm");
        dtMapa = new QDateEdit(tabM);
        dtMapa->setObjectName("dtMapa");
        dtMapa->setCalendarPopup(true);

        bm->addWidget(dtMapa);

        btnMapa = new QPushButton(tabM);
        btnMapa->setObjectName("btnMapa");

        bm->addWidget(btnMapa);


        vm->addLayout(bm);

        txtMapa = new QPlainTextEdit(tabM);
        txtMapa->setObjectName("txtMapa");
        txtMapa->setReadOnly(true);

        vm->addWidget(txtMapa);

        tabs->addTab(tabM, QString());

        v->addWidget(tabs);

        lblMensagem = new QLabel(ViewRelatorios);
        lblMensagem->setObjectName("lblMensagem");

        v->addWidget(lblMensagem);


        retranslateUi(ViewRelatorios);

        QMetaObject::connectSlotsByName(ViewRelatorios);
    } // setupUi

    void retranslateUi(QWidget *ViewRelatorios)
    {
        btnVoltar->setText(QCoreApplication::translate("ViewRelatorios", "< Voltar", nullptr));
        title->setText(QCoreApplication::translate("ViewRelatorios", "Relat\303\263rios operacionais", nullptr));
        lblExplicacaoRelatorio->setText(QCoreApplication::translate("ViewRelatorios", "Selecione a data e o destino da viagem para gerar a lista de passageiros.", nullptr));
        lDataRelatorio->setText(QCoreApplication::translate("ViewRelatorios", "Data da viagem", nullptr));
        dtRelatorioViagem->setDisplayFormat(QCoreApplication::translate("ViewRelatorios", "dd-MM-yyyy", nullptr));
        lDestinoRelatorio->setText(QCoreApplication::translate("ViewRelatorios", "Destino", nullptr));
        btnExportarPlanilhaViagem->setText(QCoreApplication::translate("ViewRelatorios", "Exportar planilha", nullptr));
        btnGerarPdfViagem->setText(QCoreApplication::translate("ViewRelatorios", "Gerar PDF", nullptr));
        tabs->setTabText(tabs->indexOf(tabRelatorioViagem), QCoreApplication::translate("ViewRelatorios", "Relat\303\263rio da viagem", nullptr));
        txtCpfHistorico->setPlaceholderText(QCoreApplication::translate("ViewRelatorios", "CPF do paciente", nullptr));
        btnHistorico->setText(QCoreApplication::translate("ViewRelatorios", "Gerar hist\303\263rico", nullptr));
        tabs->setTabText(tabs->indexOf(tabH), QCoreApplication::translate("ViewRelatorios", "Hist\303\263rico do paciente", nullptr));
        dtInicio->setDisplayFormat(QCoreApplication::translate("ViewRelatorios", "dd-MM-yyyy", nullptr));
        dtFim->setDisplayFormat(QCoreApplication::translate("ViewRelatorios", "dd-MM-yyyy", nullptr));
        btnVolume->setText(QCoreApplication::translate("ViewRelatorios", "Calcular", nullptr));
        l1->setText(QCoreApplication::translate("ViewRelatorios", "Pacientes", nullptr));
        lblPacientes->setText(QCoreApplication::translate("ViewRelatorios", "0", nullptr));
        l2->setText(QCoreApplication::translate("ViewRelatorios", "Acompanhantes", nullptr));
        lblAcompanhantes->setText(QCoreApplication::translate("ViewRelatorios", "0", nullptr));
        l3->setText(QCoreApplication::translate("ViewRelatorios", "Total de pessoas", nullptr));
        lblPessoas->setText(QCoreApplication::translate("ViewRelatorios", "0", nullptr));
        tabs->setTabText(tabs->indexOf(tabV), QCoreApplication::translate("ViewRelatorios", "Volume", nullptr));
        dtMapa->setDisplayFormat(QCoreApplication::translate("ViewRelatorios", "dd-MM-yyyy", nullptr));
        btnMapa->setText(QCoreApplication::translate("ViewRelatorios", "Gerar mapa", nullptr));
        tabs->setTabText(tabs->indexOf(tabM), QCoreApplication::translate("ViewRelatorios", "Mapa di\303\241rio", nullptr));
        (void)ViewRelatorios;
    } // retranslateUi

};

namespace Ui {
    class ViewRelatorios: public Ui_ViewRelatorios {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWRELATORIOS_H
