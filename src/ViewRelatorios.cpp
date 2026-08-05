#include "ViewRelatorios.h"

#include "CpfUtils.h"
#include "ReportExporter.h"
#include "ui_ViewRelatorios.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QFileDialog>
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
    connect(ui->dtRelatorioViagem, &QDateEdit::dateChanged, this, [this]
            { carregarViagensRelatorio(); });
    connect(ui->btnExportarPlanilhaViagem, &QPushButton::clicked, this, &ViewRelatorios::exportarPlanilhaViagem);
    connect(ui->btnGerarPdfViagem, &QPushButton::clicked, this, &ViewRelatorios::gerarPdfViagem);
    connect(ui->cmbDestinoRelatorio, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]
            {
                const bool selecionada = viagemRelatorioSelecionada() > 0;
                ui->btnExportarPlanilhaViagem->setEnabled(selecionada);
                ui->btnGerarPdfViagem->setEnabled(selecionada);
            });
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
    ui->dtRelatorioViagem->setDate(QDate::currentDate());
    ui->txtCpfHistorico->clear();
    ui->tblHistorico->clearContents();
    ui->tblHistorico->setRowCount(0);
    ui->lblPacientes->setText("0");
    ui->lblAcompanhantes->setText("0");
    ui->lblPessoas->setText("0");
    ui->txtMapa->clear();
    ui->lblMensagem->clear();
    carregarViagensRelatorio();
}

void ViewRelatorios::mensagem(const QString &texto)
{
    ui->lblMensagem->setStyleSheet("color:#0f5132;");
    ui->lblMensagem->setText(texto);
}

void ViewRelatorios::erro(const QString &texto)
{
    ui->lblMensagem->setStyleSheet("color:#b42318;");
    ui->lblMensagem->setText(texto);
}

int ViewRelatorios::viagemRelatorioSelecionada() const
{
    return ui->cmbDestinoRelatorio->currentData().toInt();
}

void ViewRelatorios::carregarViagensRelatorio()
{
    ui->cmbDestinoRelatorio->clear();
    ui->cmbDestinoRelatorio->addItem("Selecione o destino", 0);
    ui->btnExportarPlanilhaViagem->setEnabled(false);
    ui->btnGerarPdfViagem->setEnabled(false);

    try
    {
        const auto viagens = repo.listarPorData(ui->dtRelatorioViagem->date().toString("dd-MM-yyyy").toStdString());
        for (const auto &viagem : viagens)
        {
            const QString descricao = QString::fromStdString(viagem.cidadeDestino) + " - " +
                                      QString::fromStdString(viagem.veiculoPlaca) + " (viagem " +
                                      QString::number(viagem.id) + ")";
            ui->cmbDestinoRelatorio->addItem(descricao, viagem.id);
        }
        mensagem(viagens.empty() ? "Nenhuma viagem encontrada para a data selecionada."
                                  : "Selecione o destino da viagem para exportar o relatorio.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "carregar os destinos para relatorio");
    }
    catch (const std::exception &)
    {
        erro("Nao foi possivel carregar os destinos. Verifique a conexao com o banco.");
    }
}

void ViewRelatorios::exportarPlanilhaViagem()
{
    const int viagemId = viagemRelatorioSelecionada();
    if (viagemId <= 0)
    {
        erro("Selecione o destino da viagem antes de exportar.");
        return;
    }

    try
    {
        const auto relatorio = repo.gerarRelatorioViagem(viagemId);
        if (!relatorio.has_value())
        {
            erro("A viagem selecionada nao foi encontrada.");
            return;
        }

        QString caminho = QFileDialog::getSaveFileName(
            this, "Salvar planilha da viagem", "relatorio_viagem_" + QString::number(viagemId) + ".csv",
            "Planilha CSV (*.csv)");
        if (caminho.isEmpty())
            return;
        if (!caminho.endsWith(".csv", Qt::CaseInsensitive))
            caminho += ".csv";

        QString motivo;
        if (!ReportExporter::exportarCsv(*relatorio, caminho, &motivo))
        {
            erro(motivo);
            return;
        }
        mensagem("Planilha exportada com sucesso.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "exportar a planilha");
    }
    catch (const std::exception &)
    {
        erro("Nao foi possivel exportar a planilha. Verifique a conexao com o banco.");
    }
}

void ViewRelatorios::gerarPdfViagem()
{
    const int viagemId = viagemRelatorioSelecionada();
    if (viagemId <= 0)
    {
        erro("Selecione o destino da viagem antes de gerar o PDF.");
        return;
    }

    try
    {
        const auto relatorio = repo.gerarRelatorioViagem(viagemId);
        if (!relatorio.has_value())
        {
            erro("A viagem selecionada nao foi encontrada.");
            return;
        }

        QString caminho = QFileDialog::getSaveFileName(
            this, "Salvar relatorio em PDF", "relatorio_viagem_" + QString::number(viagemId) + ".pdf",
            "Documento PDF (*.pdf)");
        if (caminho.isEmpty())
            return;
        if (!caminho.endsWith(".pdf", Qt::CaseInsensitive))
            caminho += ".pdf";

        QString motivo;
        if (!ReportExporter::exportarPdf(*relatorio, caminho, &motivo))
        {
            erro(motivo);
            return;
        }
        mensagem("PDF gerado com sucesso.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "gerar o PDF");
    }
    catch (const std::exception &)
    {
        erro("Nao foi possivel gerar o PDF. Verifique a conexao com o banco.");
    }
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
