#include "ViewPacientes.h"
#include "ui_ViewPacientes.h" // Arquivo gerado automaticamente pelo Qt a partir do .ui
#include "CpfUtils.h"
#include <QMessageBox>
#include <QRegularExpression>

ViewPacientes::ViewPacientes(Database &db, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewPacientes), m_db(db), m_repo(db)
{
    // 1. "Desenha" a tela usando o arquivo .ui
    ui->setupUi(this);

    // 2. Conecta os cliques dos botões aos seus métodos
    connect(ui->btnSalvar, &QPushButton::clicked, this, &ViewPacientes::salvarPaciente);
    connect(ui->btnAtualizar, &QPushButton::clicked, this, &ViewPacientes::atualizarPaciente);
    connect(ui->btnDeletar, &QPushButton::clicked, this, &ViewPacientes::deletarPaciente);

    // 3. Ativa as máscaras dinâmicas de CPF e Telefone
    configurarMascaras();
}

ViewPacientes::~ViewPacientes()
{
    delete ui; // Libera a interface da memória ao fechar a tela
}

void ViewPacientes::configurarMascaras()
{
    // Máscara do CPF
    connect(ui->txtCpf, &QLineEdit::textChanged, this, [this](const QString &texto)
            {
        QString numeros = texto;
        numeros.remove(QRegularExpression("[^\\d]"));
        if (numeros.length() > 11) numeros = numeros.left(11);

        QString formatado = "";
        for (int i = 0; i < numeros.length(); ++i) {
            if (i == 3 || i == 6) formatado += ".";
            else if (i == 9) formatado += "-";
            formatado += numeros[i];
        }

        if (ui->txtCpf->text() != formatado) {
            ui->txtCpf->blockSignals(true);
            ui->txtCpf->setText(formatado);
            ui->txtCpf->blockSignals(false);
        } });

    // Máscara do Telefone
    connect(ui->txtTelefone, &QLineEdit::textChanged, this, [this](const QString &texto)
            {
        QString numeros = texto;
        numeros.remove(QRegularExpression("[^\\d]"));
        if (numeros.length() > 11) numeros = numeros.left(11);

        QString formatado = "";
        for (int i = 0; i < numeros.length(); ++i) {
            if (i == 0) formatado += "(";
            else if (i == 2) formatado += ") ";
            else if (i == 7) formatado += "-";
            formatado += numeros[i];
        }

        if (ui->txtTelefone->text() != formatado) {
            ui->txtTelefone->blockSignals(true);
            ui->txtTelefone->setText(formatado);
            ui->txtTelefone->blockSignals(false);
        } });
}

void ViewPacientes::salvarPaciente()
{
    QString nome = ui->txtNome->text().trimmed();
    QString cpf = ui->txtCpf->text().trimmed();

    if (nome.isEmpty() || cpf.isEmpty())
    {
        QMessageBox::warning(this, "Atenção", "Os campos Nome e CPF são obrigatórios.");
        return;
    }

    try
    {
        if (!CpfUtils::verificar(cpf.toStdString()))
        {
            QMessageBox::warning(this, "CPF Inválido", "O CPF informado não é válido.");
            return;
        }

        ui->btnSalvar->setEnabled(false);

        Paciente p;
        p.nomeCompleto = nome.toStdString();
        p.cpf = cpf.toStdString();
        p.telefone = ui->txtTelefone->text().toStdString();
        p.endereco = ui->txtEndereco->text().toStdString();

        int idGerado = m_repo.cadastrar(p);

        QMessageBox::information(this, "Sucesso", "Paciente cadastrado com sucesso! ID: " + QString::number(idGerado));

        ui->txtNome->clear();
        ui->txtCpf->clear();
        ui->txtTelefone->clear();
        ui->txtEndereco->clear();
    }
    catch (const pqxx::sql_error &e)
    {
        std::string sqlState = e.sqlstate();
        if (sqlState == "23505")
        {
            QMessageBox::warning(this, "Atenção", "Este CPF já está cadastrado.");
        }
        else if (sqlState == "23503")
        {
            QMessageBox::warning(this, "Atenção", "Operação negada: existem registros vinculados.");
        }
        else
        {
            QMessageBox::critical(this, "Erro no Banco", QString("Erro SQL: ") + e.what());
        }
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Erro Crítico", QString("Ocorreu um erro: ") + e.what());
    }

    ui->btnSalvar->setEnabled(true);
}

void ViewPacientes::atualizarPaciente()
{
    try
    {
        QString cpfDigitado = ui->txtCpf->text();

        if (!CpfUtils::verificar(cpfDigitado.toStdString()))
        {
            QMessageBox::warning(this, "CPF Inválido", "O CPF informado não é válido.");
            return;
        }

        ui->btnAtualizar->setEnabled(false);

        Paciente p;
        p.nomeCompleto = ui->txtNome->text().toStdString();
        p.cpf = cpfDigitado.toStdString();
        p.telefone = ui->txtTelefone->text().toStdString();
        p.endereco = ui->txtEndereco->text().toStdString();

        m_repo.atualizar(p);

        QMessageBox::information(this, "Sucesso", "Dados do paciente atualizados com sucesso!");
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao atualizar: ") + e.what());
    }

    ui->btnAtualizar->setEnabled(true);
}

void ViewPacientes::deletarPaciente()
{
    try
    {
        QString cpfDigitado = ui->txtCpf->text();

        if (cpfDigitado.isEmpty())
        {
            QMessageBox::warning(this, "Atenção", "Informe o CPF do paciente que deseja excluir.");
            return;
        }

        QMessageBox::StandardButton resposta = QMessageBox::question(this, "Confirmar Exclusão",
                                                                     "Tem certeza que deseja excluir permanentemente este paciente?",
                                                                     QMessageBox::Yes | QMessageBox::No);

        if (resposta == QMessageBox::Yes)
        {
            ui->btnDeletar->setEnabled(false);
            m_repo.deletar(cpfDigitado.toStdString());
            QMessageBox::information(this, "Sucesso", "Paciente excluído com sucesso!");

            ui->txtNome->clear();
            ui->txtCpf->clear();
            ui->txtTelefone->clear();
            ui->txtEndereco->clear();
        }
    }
    catch (const pqxx::sql_error &e)
    {
        std::string sqlState = e.sqlstate();
        if (sqlState == "23503")
        {
            QMessageBox::warning(this, "Exclusão Negada", "Não é possível excluir este paciente pois ele possui viagens cadastradas.");
        }
        else
        {
            QMessageBox::critical(this, "Erro no Banco", QString("Erro SQL: ") + e.what());
        }
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao excluir: ") + e.what());
    }

    ui->btnDeletar->setEnabled(true);
}