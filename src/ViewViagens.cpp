#include "ViewViagens.h"

#include "CpfUtils.h"
#include "ui_ViewViagens.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidgetItem>

ViewViagens::ViewViagens(Database &db, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ViewViagens),
      pacientes(db),
      acompanhantes(db),
      motoristas(db),
      veiculos(db),
      viagens(db)
{
    ui->setupUi(this);
    configurarTabelas();

    connect(ui->btnVoltar, &QPushButton::clicked, this, &ViewViagens::voltarSolicitado);
    connect(ui->btnBuscarPaciente, &QPushButton::clicked, this, &ViewViagens::buscarPaciente);
    connect(ui->btnBuscarAcompanhante, &QPushButton::clicked, this, &ViewViagens::buscarAcompanhante);
    connect(ui->btnAdicionar, &QPushButton::clicked, this, &ViewViagens::adicionarPassageiro);
    connect(ui->btnRemover, &QPushButton::clicked, this, &ViewViagens::removerPassageiro);
    connect(ui->btnSalvar, &QPushButton::clicked, this, &ViewViagens::salvar);
    connect(ui->btnListar, &QPushButton::clicked, this, &ViewViagens::listar);
    connect(ui->tblViagens, &QTableWidget::cellClicked, this, &ViewViagens::carregarSelecionada);
    connect(ui->btnExcluir, &QPushButton::clicked, this, &ViewViagens::excluirViagem);
    connect(ui->btnLimpar, &QPushButton::clicked, this, &ViewViagens::limpar);
    connect(ui->txtCpfPaciente, &QLineEdit::returnPressed, this, &ViewViagens::buscarPaciente);
    connect(ui->txtCpfAcompanhante, &QLineEdit::returnPressed, this, &ViewViagens::buscarAcompanhante);

    prepararTela();
}

ViewViagens::~ViewViagens()
{
    delete ui;
}

void ViewViagens::configurarTabelas()
{
    ui->tblPassageiros->setHorizontalHeaderLabels({"Paciente", "CPF", "Acompanhante"});
    ui->tblPassageiros->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblPassageiros->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPassageiros->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblPassageiros->setAlternatingRowColors(true);
    ui->tblPassageiros->verticalHeader()->setVisible(false);
    ui->tblPassageiros->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblPassageiros->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tblPassageiros->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    ui->tblViagens->setHorizontalHeaderLabels({"Código", "Destino", "Veículo", "Pacientes"});
    ui->tblViagens->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblViagens->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblViagens->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblViagens->setAlternatingRowColors(true);
    ui->tblViagens->verticalHeader()->setVisible(false);
    ui->tblViagens->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tblViagens->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tblViagens->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tblViagens->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}

void ViewViagens::mensagem(const QString &texto)
{
    ui->lblMensagem->setStyleSheet("color:#0f5132;");
    ui->lblMensagem->setText(texto);
}

void ViewViagens::erro(const QString &texto)
{
    ui->lblMensagem->setStyleSheet("color:#b42318;");
    ui->lblMensagem->setText(texto);
}

void ViewViagens::erroSql(const pqxx::sql_error &erroBanco, const QString &acao)
{
    const std::string codigo = erroBanco.sqlstate();
    QString detalhe;
    QString titulo = "Operação não concluída";

    if (codigo == "23503")
    {
        detalhe = "Um paciente, acompanhante, veículo ou motorista vinculado não está mais disponível. Atualize os dados e tente novamente.";
    }
    else if (codigo == "23505")
    {
        detalhe = "Há um vínculo de passageiro duplicado nesta viagem.";
    }
    else if (codigo == "23514" || codigo == "22001" || codigo == "22007")
    {
        detalhe = "Os dados não atendem às regras de integridade exigidas pelo sistema.";
    }
    else
    {
        titulo = "Erro no banco de dados";
        detalhe = QString("Não foi possível %1. Tente novamente ou contate o suporte.").arg(acao);
    }

    erro(detalhe);
    QMessageBox::warning(this, titulo, detalhe);
}

void ViewViagens::prepararTela()
{
    limpar();
}

void ViewViagens::carregarRecursos()
{
    try
    {
        ui->cmbVeiculo->clear();
        ui->cmbMotorista->clear();

        for (const auto &veiculo : veiculos.listarTodos())
        {
            ui->cmbVeiculo->addItem(
                QString::fromStdString(veiculo.placa + " — " + veiculo.modelo), veiculo.id);
        }
        for (const auto &motorista : motoristas.listarTodos())
        {
            ui->cmbMotorista->addItem(
                QString::fromStdString(motorista.nome + " — " + motorista.cpf), motorista.id);
        }

        if (ui->cmbVeiculo->count() == 0 || ui->cmbMotorista->count() == 0)
        {
            erro("Cadastre ao menos um veículo e um motorista antes de criar uma viagem.");
        }
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "carregar veículos e motoristas");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível carregar veículos e motoristas. Verifique a conexão com o banco.");
    }
}

void ViewViagens::limpar()
{
    viagemId = 0;
    passageiros.clear();
    pacientePendente.reset();
    acompanhantePendente.reset();

    ui->dtViagem->setDate(QDate::currentDate());
    ui->txtDestino->clear();
    ui->txtCpfPaciente->clear();
    ui->txtCpfAcompanhante->clear();
    ui->lblPaciente->clear();
    ui->lblAcompanhante->clear();
    ui->tblPassageiros->setRowCount(0);
    ui->lblMensagem->clear();
    ui->btnExcluir->setEnabled(false);
    carregarRecursos();
}

void ViewViagens::buscarPaciente()
{
    pacientePendente.reset();
    ui->lblPaciente->clear();

    const std::string cpf = CpfUtils::normalizar(ui->txtCpfPaciente->text().toStdString());
    if (!CpfUtils::verificar(cpf))
    {
        erro("Informe um CPF de paciente válido.");
        return;
    }

    try
    {
        pacientePendente = pacientes.buscarPorCPF(cpf);
        if (!pacientePendente.has_value())
        {
            erro("Paciente não encontrado. Confira o CPF ou cadastre o paciente antes de adicioná-lo.");
            return;
        }
        mensagem("Paciente localizado.");
        ui->lblPaciente->setText(QString::fromStdString(pacientePendente->nomeCompleto));
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "buscar o paciente");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível buscar o paciente. Verifique a conexão com o banco.");
    }
}

void ViewViagens::buscarAcompanhante()
{
    acompanhantePendente.reset();
    ui->lblAcompanhante->clear();

    if (ui->txtCpfAcompanhante->text().trimmed().isEmpty())
    {
        mensagem("O paciente será incluído sem acompanhante.");
        ui->lblAcompanhante->setText("Sem acompanhante");
        return;
    }

    const std::string cpf = CpfUtils::normalizar(ui->txtCpfAcompanhante->text().toStdString());
    if (!CpfUtils::verificar(cpf))
    {
        erro("Informe um CPF de acompanhante válido.");
        return;
    }

    try
    {
        acompanhantePendente = acompanhantes.buscarPorCPF(cpf);
        if (!acompanhantePendente.has_value())
        {
            erro("Acompanhante não encontrado. Deixe o campo vazio se o paciente viajar sem acompanhante.");
            return;
        }
        mensagem("Acompanhante localizado.");
        ui->lblAcompanhante->setText(QString::fromStdString(acompanhantePendente->nomeCompleto));
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "buscar o acompanhante");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível buscar o acompanhante. Verifique a conexão com o banco.");
    }
}

void ViewViagens::adicionarPassageiro()
{
    if (!pacientePendente.has_value())
    {
        erro("Busque e confirme o paciente antes de adicioná-lo à viagem.");
        return;
    }

    for (const auto &passageiro : passageiros)
    {
        if (passageiro.relacao.pacienteId == pacientePendente->id)
        {
            erro("Este paciente já foi incluído na viagem.");
            return;
        }
    }

    PassageiroViagemDetalhe detalhe;
    detalhe.relacao.pacienteId = pacientePendente->id;
    detalhe.pacienteNome = pacientePendente->nomeCompleto;
    detalhe.pacienteCpf = pacientePendente->cpf;
    if (acompanhantePendente.has_value())
    {
        detalhe.relacao.acompanhanteId = acompanhantePendente->id;
        detalhe.acompanhanteNome = acompanhantePendente->nomeCompleto;
        detalhe.acompanhanteCpf = acompanhantePendente->cpf;
    }

    passageiros.push_back(detalhe);
    desenharPassageiros();
    pacientePendente.reset();
    acompanhantePendente.reset();
    ui->txtCpfPaciente->clear();
    ui->txtCpfAcompanhante->clear();
    ui->lblPaciente->clear();
    ui->lblAcompanhante->clear();
    mensagem("Passageiro incluído na viagem.");
}

void ViewViagens::desenharPassageiros()
{
    ui->tblPassageiros->setRowCount(static_cast<int>(passageiros.size()));
    for (int linha = 0; linha < static_cast<int>(passageiros.size()); ++linha)
    {
        const auto &passageiro = passageiros[linha];
        ui->tblPassageiros->setItem(linha, 0, new QTableWidgetItem(QString::fromStdString(passageiro.pacienteNome)));
        ui->tblPassageiros->setItem(linha, 1, new QTableWidgetItem(QString::fromStdString(passageiro.pacienteCpf)));
        ui->tblPassageiros->setItem(
            linha, 2,
            new QTableWidgetItem(passageiro.acompanhanteNome.has_value()
                                     ? QString::fromStdString(*passageiro.acompanhanteNome)
                                     : "—"));
    }
}

void ViewViagens::removerPassageiro()
{
    const int linha = ui->tblPassageiros->currentRow();
    if (linha < 0 || linha >= static_cast<int>(passageiros.size()))
    {
        erro("Selecione o paciente que deseja remover da viagem.");
        return;
    }

    passageiros.erase(passageiros.begin() + linha);
    desenharPassageiros();
    mensagem("Passageiro removido da viagem.");
}

Viagem ViewViagens::montarViagem() const
{
    Viagem viagem;
    viagem.id = viagemId;
    viagem.dataViagem = ui->dtViagem->date().toString("dd-MM-yyyy").toStdString();
    viagem.cidadeDestino = ui->txtDestino->text().trimmed().toStdString();
    viagem.veiculoId = ui->cmbVeiculo->currentData().toInt();
    viagem.motoristaId = ui->cmbMotorista->currentData().toInt();
    for (const auto &passageiro : passageiros)
        viagem.passageiros.push_back(passageiro.relacao);
    return viagem;
}

void ViewViagens::salvar()
{
    const Viagem viagem = montarViagem();
    if (viagem.cidadeDestino.empty())
    {
        erro("Informe a cidade de destino.");
        ui->txtDestino->setFocus();
        return;
    }
    if (viagem.veiculoId <= 0 || viagem.motoristaId <= 0)
    {
        erro("Selecione um veículo e um motorista válidos.");
        return;
    }
    if (viagem.passageiros.empty())
    {
        erro("Inclua ao menos um paciente na viagem.");
        return;
    }

    ui->btnSalvar->setEnabled(false);
    try
    {
        if (viagemId == 0)
        {
            const int id = viagens.cadastrarViagem(viagem);
            limpar();
            mensagem("Viagem cadastrada com sucesso. Código: " + QString::number(id) + ".");
        }
        else if (!viagens.atualizarViagem(viagem))
        {
            limpar();
            QMessageBox::warning(this, "Viagem não encontrada",
                                 "A viagem foi removida por outro atendimento. Faça uma nova busca.");
        }
        else
        {
            limpar();
            mensagem("Viagem atualizada com sucesso.");
        }
    }
    catch (const RegraNegocioException &erroRegra)
    {
        erro(QString::fromUtf8(erroRegra.what()));
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "salvar a viagem");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível salvar a viagem. Verifique a conexão com o banco.");
    }

    ui->btnSalvar->setEnabled(true);
}

void ViewViagens::listar()
{
    ui->tblViagens->setRowCount(0);
    try
    {
        const auto lista = viagens.listarPorData(ui->dtViagem->date().toString("dd-MM-yyyy").toStdString());
        ui->tblViagens->setRowCount(static_cast<int>(lista.size()));
        for (int linha = 0; linha < static_cast<int>(lista.size()); ++linha)
        {
            const auto &viagem = lista[linha];
            auto *codigo = new QTableWidgetItem(QString::number(viagem.id));
            codigo->setData(Qt::UserRole, viagem.id);
            ui->tblViagens->setItem(linha, 0, codigo);
            ui->tblViagens->setItem(linha, 1, new QTableWidgetItem(QString::fromStdString(viagem.cidadeDestino)));
            ui->tblViagens->setItem(linha, 2, new QTableWidgetItem(QString::fromStdString(viagem.veiculoPlaca)));
            ui->tblViagens->setItem(linha, 3, new QTableWidgetItem(QString::number(viagem.totalPacientes)));
        }

        mensagem(lista.empty() ? "Nenhuma viagem cadastrada para esta data."
                               : QString::number(lista.size()) + " viagem(ns) encontrada(s). Selecione uma para editar.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "listar as viagens");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível listar as viagens. Verifique a conexão com o banco.");
    }
}

void ViewViagens::carregarSelecionada(int linha, int)
{
    if (linha < 0)
        return;

    const QTableWidgetItem *itemCodigo = ui->tblViagens->item(linha, 0);
    if (itemCodigo == nullptr)
        return;

    try
    {
        const int id = itemCodigo->data(Qt::UserRole).toInt();
        const auto detalhe = viagens.buscarPorId(id);
        if (!detalhe.has_value())
        {
            erro("A viagem não foi encontrada. Atualize a lista.");
            return;
        }

        viagemId = id;
        ui->dtViagem->setDate(QDate::fromString(QString::fromStdString(detalhe->viagem.dataViagem), "dd-MM-yyyy"));
        ui->txtDestino->setText(QString::fromStdString(detalhe->viagem.cidadeDestino));
        for (int indice = 0; indice < ui->cmbVeiculo->count(); ++indice)
        {
            if (ui->cmbVeiculo->itemData(indice).toInt() == detalhe->viagem.veiculoId)
                ui->cmbVeiculo->setCurrentIndex(indice);
        }
        for (int indice = 0; indice < ui->cmbMotorista->count(); ++indice)
        {
            if (ui->cmbMotorista->itemData(indice).toInt() == detalhe->viagem.motoristaId)
                ui->cmbMotorista->setCurrentIndex(indice);
        }

        passageiros = detalhe->passageiros;
        desenharPassageiros();
        ui->btnExcluir->setEnabled(true);
        mensagem("Viagem carregada. Revise os dados antes de salvar as alterações.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "carregar a viagem");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível carregar a viagem. Verifique a conexão com o banco.");
    }
}

void ViewViagens::excluirViagem()
{
    if (viagemId == 0)
    {
        erro("Selecione uma viagem antes de solicitar a exclusão.");
        return;
    }

    const auto resposta = QMessageBox::question(
        this,
        "Confirmar exclusão",
        "Deseja excluir definitivamente esta viagem?\n\nOs vínculos de passageiros desta viagem também serão removidos. Esta ação não poderá ser desfeita.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (resposta != QMessageBox::Yes)
        return;

    ui->btnExcluir->setEnabled(false);
    try
    {
        if (!viagens.deletarViagem(viagemId))
        {
            limpar();
            QMessageBox::warning(this, "Viagem não encontrada",
                                 "A viagem foi removida por outro atendimento. Atualize a lista.");
            return;
        }

        limpar();
        mensagem("Viagem excluída com sucesso.");
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "excluir a viagem");
    }
    catch (const std::exception &)
    {
        erro("Não foi possível excluir a viagem. Verifique a conexão com o banco.");
    }

    if (viagemId != 0)
        ui->btnExcluir->setEnabled(true);
}
