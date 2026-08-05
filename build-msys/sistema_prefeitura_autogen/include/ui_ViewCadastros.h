/********************************************************************************
** Form generated from reading UI file 'ViewCadastros.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWCADASTROS_H
#define UI_VIEWCADASTROS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewCadastros
{
public:
    QVBoxLayout *v;
    QHBoxLayout *top;
    QPushButton *btnVoltar;
    QLabel *titulo;
    QGroupBox *grupo;
    QFormLayout *form;
    QLabel *lTipo;
    QComboBox *cmbTipo;
    QLabel *lblChave;
    QLineEdit *txtChave;
    QLabel *lblNome;
    QLineEdit *txtNome;
    QLabel *lblTelefone;
    QLineEdit *txtTelefone;
    QLabel *lblMensagem;
    QHBoxLayout *actions;
    QPushButton *btnLimpar;
    QPushButton *btnBuscar;
    QPushButton *btnSalvar;
    QPushButton *btnAtualizar;
    QPushButton *btnExcluir;

    void setupUi(QWidget *ViewCadastros)
    {
        if (ViewCadastros->objectName().isEmpty())
            ViewCadastros->setObjectName("ViewCadastros");
        v = new QVBoxLayout(ViewCadastros);
        v->setObjectName("v");
        v->setContentsMargins(48, 36, 48, 36);
        top = new QHBoxLayout();
        top->setObjectName("top");
        btnVoltar = new QPushButton(ViewCadastros);
        btnVoltar->setObjectName("btnVoltar");

        top->addWidget(btnVoltar);

        titulo = new QLabel(ViewCadastros);
        titulo->setObjectName("titulo");
        titulo->setAlignment(Qt::AlignCenter);
        titulo->setStyleSheet(QString::fromUtf8("font-size:24px;font-weight:700;color:#1d2939;"));

        top->addWidget(titulo);


        v->addLayout(top);

        grupo = new QGroupBox(ViewCadastros);
        grupo->setObjectName("grupo");
        form = new QFormLayout(grupo);
        form->setObjectName("form");
        lTipo = new QLabel(grupo);
        lTipo->setObjectName("lTipo");

        form->setWidget(0, QFormLayout::ItemRole::LabelRole, lTipo);

        cmbTipo = new QComboBox(grupo);
        cmbTipo->addItem(QString());
        cmbTipo->addItem(QString());
        cmbTipo->addItem(QString());
        cmbTipo->setObjectName("cmbTipo");

        form->setWidget(0, QFormLayout::ItemRole::FieldRole, cmbTipo);

        lblChave = new QLabel(grupo);
        lblChave->setObjectName("lblChave");

        form->setWidget(1, QFormLayout::ItemRole::LabelRole, lblChave);

        txtChave = new QLineEdit(grupo);
        txtChave->setObjectName("txtChave");

        form->setWidget(1, QFormLayout::ItemRole::FieldRole, txtChave);

        lblNome = new QLabel(grupo);
        lblNome->setObjectName("lblNome");

        form->setWidget(2, QFormLayout::ItemRole::LabelRole, lblNome);

        txtNome = new QLineEdit(grupo);
        txtNome->setObjectName("txtNome");

        form->setWidget(2, QFormLayout::ItemRole::FieldRole, txtNome);

        lblTelefone = new QLabel(grupo);
        lblTelefone->setObjectName("lblTelefone");

        form->setWidget(3, QFormLayout::ItemRole::LabelRole, lblTelefone);

        txtTelefone = new QLineEdit(grupo);
        txtTelefone->setObjectName("txtTelefone");

        form->setWidget(3, QFormLayout::ItemRole::FieldRole, txtTelefone);


        v->addWidget(grupo);

        lblMensagem = new QLabel(ViewCadastros);
        lblMensagem->setObjectName("lblMensagem");
        lblMensagem->setMinimumSize(QSize(0, 24));

        v->addWidget(lblMensagem);

        actions = new QHBoxLayout();
        actions->setObjectName("actions");
        btnLimpar = new QPushButton(ViewCadastros);
        btnLimpar->setObjectName("btnLimpar");

        actions->addWidget(btnLimpar);

        btnBuscar = new QPushButton(ViewCadastros);
        btnBuscar->setObjectName("btnBuscar");

        actions->addWidget(btnBuscar);

        btnSalvar = new QPushButton(ViewCadastros);
        btnSalvar->setObjectName("btnSalvar");

        actions->addWidget(btnSalvar);

        btnAtualizar = new QPushButton(ViewCadastros);
        btnAtualizar->setObjectName("btnAtualizar");

        actions->addWidget(btnAtualizar);

        btnExcluir = new QPushButton(ViewCadastros);
        btnExcluir->setObjectName("btnExcluir");

        actions->addWidget(btnExcluir);


        v->addLayout(actions);


        retranslateUi(ViewCadastros);

        QMetaObject::connectSlotsByName(ViewCadastros);
    } // setupUi

    void retranslateUi(QWidget *ViewCadastros)
    {
        ViewCadastros->setWindowTitle(QCoreApplication::translate("ViewCadastros", "Cadastros auxiliares", nullptr));
        btnVoltar->setText(QCoreApplication::translate("ViewCadastros", "< Voltar", nullptr));
        titulo->setText(QCoreApplication::translate("ViewCadastros", "Cadastros auxiliares", nullptr));
        grupo->setTitle(QCoreApplication::translate("ViewCadastros", "Gerenciar cadastro", nullptr));
        lTipo->setText(QCoreApplication::translate("ViewCadastros", "Tipo", nullptr));
        cmbTipo->setItemText(0, QCoreApplication::translate("ViewCadastros", "Motorista", nullptr));
        cmbTipo->setItemText(1, QCoreApplication::translate("ViewCadastros", "Ve\303\255culo", nullptr));
        cmbTipo->setItemText(2, QCoreApplication::translate("ViewCadastros", "Auxiliar de viagem", nullptr));

        lblChave->setText(QCoreApplication::translate("ViewCadastros", "CPF *", nullptr));
        lblNome->setText(QCoreApplication::translate("ViewCadastros", "Nome completo *", nullptr));
        lblTelefone->setText(QCoreApplication::translate("ViewCadastros", "Telefone", nullptr));
        btnLimpar->setText(QCoreApplication::translate("ViewCadastros", "Limpar", nullptr));
        btnBuscar->setText(QCoreApplication::translate("ViewCadastros", "Buscar", nullptr));
        btnSalvar->setText(QCoreApplication::translate("ViewCadastros", "Salvar", nullptr));
        btnAtualizar->setText(QCoreApplication::translate("ViewCadastros", "Atualizar", nullptr));
        btnExcluir->setText(QCoreApplication::translate("ViewCadastros", "Excluir", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewCadastros: public Ui_ViewCadastros {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWCADASTROS_H
