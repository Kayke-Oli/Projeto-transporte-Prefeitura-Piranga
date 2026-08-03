#include "ViewRelatorios.h"

#include "CpfUtils.h"
#include "ui_ViewRelatorios.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidgetItem>

ViewRelatorios::ViewRelatorios(Database &db, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewRelatorios), repo(db)
{
    ui->setupUi(this);
    configurarTabelas();

    connect(ui->btnVoltar, &QPushButton::clicked, this, &ViewRelatorios::voltarSolicitado);
    connect(ui->btnHistorico, &QPushButton::clicked, this, &ViewRelatorios::historico);
    connect(ui->btnVolume, &QPushButton::clicked, this, &ViewRelatorios::volume);
    connect(ui->btnMapa, &QPushButton::clicked, this, &ViewRelatorios::mapa);
    connect(ui->txtCpfHistorico, &QLineEdit::returnPressed, this, &ViewRelatorios::historico);

    prepararTela();
}

ViewRelatorios::~ViewRelatorios()
{
    delete ui;
}

void ViewRelatorios::configurarTabelas()
{
    ui->tblHistorico->setHorizontalHeaderLabels({"Data", "Destino", "Motorista", "Acompanhante"});
    ui->tblHistorico->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblHistorico->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblHistorico->setAlternatingRowColors(true);
    ui->tblHistorico->verticalHeader()->setVisible(false);
    ui->tblHistorico->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tblHistorico->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tblHistorico->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tblHistorico->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void ViewRelatorios::prepararTela()
{
    ui->dtInicio->setDate(QDate::currentDate().addMonths(-1));
    ui->dtFim->setDate(QDate::currentDate());
    ui->dtMapa->setDate(QDate::currentDate());
    ui->txtCpfHistorico->clear();
    ui->tblHistorico->clearContents();
    ui->tblHistorico->setRowCount(0);
    ui->lblPacientes->setText("0");
    ui->lblAcompanhantes->setText("0");
    ui->lblPessoas->setText("0");
    ui->txtMapa->clear();
    ui->lblMensagem->clear();
}

void ViewRelatorios::erro(const QString &texto)
{
    ui->lblMensagem->setStyleSheet("color:#b42318;");
    ui->lblMensagem->setText(texto);
}

void ViewRelatorios::erroSql(const pqxx::sql_error &erroBanco, const QString &acao)
{
    QString mensagem;
    const std::string codigo = erroBanco.sqlstate();
    if (codigo == "23514" || codigo == "22007")
        mensagem = "O período informado não atende às regras de integridade do banco.";
    else
        mensagem = QString("Não foi possível %1. Tente novamente ou contate o suporte.").arg(acao);

    erro(mensagem);
    QMessageBox::warning(this, "Relatório não gerado", mensagem);
}

void ViewRelatorios::historico()
{
    const std::string cpf = CpfUtils::normalizar(ui->txtCpfHistorico->text().toStdString());
    if (!CpfUtils::verificar(cpf))
    {
        erro("Informe um CPF válido para gerar o histórico.");
        ui->txtCpfHistorico->setFocus();
        return;
    }

    try
    {
        const auto itens = repo.gerarRelatorioHistoricoPaciente(cpf);
        ui->tblHistorico->clearContents();
        ui->tblHistorico->setRowCount(static_cast<int>(itens.size()));
        for (int linha = 0; linha < static_cast<int>(itens.size()); ++linha)
        {
            const auto &item = itens[linha];
            ui->tblHistorico->setItem(linha, 0, new QTableWidgetItem(QString::fromStdString(item.dataViagem)));
            ui->tblHistorico->setItem(linha, 1, new QTableWidgetItem(QString::fromStdString(item.cidadeDestino)));
            ui->tblHistorico->setItem(linha, 2, new QTableWidgetItem(QString::fromStdString(item.motorista)));
            ui->tblHistorico->setItem(
                linha, 3,
                new QTableWidgetItem(item.acompanhante.has_value() ? QString::fromStdString(*item.acompanhante) : "—"));
        }
        ui->lblMensagem->setStyleSheet("color:#0f5132;");
        ui->lblMensagem->setText(itens.empty() ? "Nenhuma viagem encontrada para este paciente."
                                               : QString::number(itens.size()) + " viagem(ns) encontrada(s).");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "gerar o histórico do paciente");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível gerar o histórico. Verifique a conexão com o banco.");
    }
}

void ViewRelatorios::volume()
{
    if (ui->dtInicio->date() > ui->dtFim->date())
    {
        erro("A data inicial não pode ser posterior à data final.");
        return;
    }

    try
    {
        const auto resultado = repo.gerarRelatorioVolumePassageiros(
            ui->dtInicio->date().toString("dd-MM-yyyy").toStdString(),
            ui->dtFim->date().toString("dd-MM-yyyy").toStdString());
        ui->lblPacientes->setText(QString::number(resultado.totalPacientes));
        ui->lblAcompanhantes->setText(QString::number(resultado.totalAcompanhantes));
        ui->lblPessoas->setText(QString::number(resultado.totalPessoas));
        ui->lblMensagem->setStyleSheet("color:#0f5132;");
        ui->lblMensagem->setText("Volume calculado para o período selecionado.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "calcular o volume de passageiros");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível calcular o volume. Verifique a conexão com o banco.");
    }
}

void ViewRelatorios::mapa()
{
    try
    {
        const auto itens = repo.gerarMapaViagemDiario(ui->dtMapa->date().toString("dd-MM-yyyy").toStdString());
        QString texto;
        for (const auto &viagem : itens)
        {
            texto += QString("VIAGEM %1 — %2\nVeículo: %3 (%4) | Motorista: %5\n")
                         .arg(viagem.viagemId)
                         .arg(QString::fromStdString(viagem.cidadeDestino))
                         .arg(QString::fromStdString(viagem.veiculoPlaca))
                         .arg(QString::fromStdString(viagem.veiculoModelo))
                         .arg(QString::fromStdString(viagem.motorista));
            for (const auto &passageiro : viagem.passageiros)
            {
                texto += " • " + QString::fromStdString(passageiro.pacienteNome);
                if (passageiro.pacienteTelefone.has_value())
                    texto += " | telefone: " + QString::fromStdString(*passageiro.pacienteTelefone);
                if (passageiro.acompanhanteNome.has_value())
                    texto += " | acompanhante: " + QString::fromStdString(*passageiro.acompanhanteNome);
                texto += '\n';
            }
            texto += '\n';
        }

        ui->txtMapa->setPlainText(texto.isEmpty() ? "Nenhuma viagem encontrada para esta data." : texto);
        ui->lblMensagem->setStyleSheet("color:#0f5132;");
        ui->lblMensagem->setText(texto.isEmpty() ? "Mapa diário sem viagens." : "Mapa diário gerado com sucesso.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "gerar o mapa diário");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível gerar o mapa diário. Verifique a conexão com o banco.");
    }
}
