#include "ViewViagens.h"

#include "CpfUtils.h"
#include "ReportExporter.h"
#include "ui_ViewViagens.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QTableWidgetItem>

#include <algorithm>

namespace
{
    QString somenteDigitos(const QString &texto)
    {
        QString numeros;
        numeros.reserve(texto.size());
        for (const QChar caractere : texto)
        {
            if (caractere.isDigit())
                numeros += caractere;
        }
        return numeros;
    }

    QString formatarCpf(const QString &texto)
    {
        const QString numeros = somenteDigitos(texto).left(11);
        QString resultado;
        for (qsizetype indice = 0; indice < numeros.size(); ++indice)
        {
            if (indice == 3 || indice == 6)
                resultado += ".";
            else if (indice == 9)
                resultado += "-";
            resultado += numeros[indice];
        }
        return resultado;
    }

    QString formatarTelefone(const QString &texto)
    {
        const QString numeros = somenteDigitos(texto).left(11);
        if (numeros.isEmpty())
            return {};
        if (numeros.size() <= 2)
            return "(" + numeros;

        const QString ddd = numeros.left(2);
        const QString numero = numeros.mid(2);
        const int tamanhoPrimeiroBloco = numeros.size() == 11 ? 5 : 4;
        if (numero.size() <= tamanhoPrimeiroBloco)
            return "(" + ddd + ") " + numero;

        return "(" + ddd + ") " + numero.left(tamanhoPrimeiroBloco) + "-" + numero.mid(tamanhoPrimeiroBloco);
    }
}

ViewViagens::ViewViagens(Database &db, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ViewViagens),
      pacientes(db),
      acompanhantes(db),
      auxiliares(db),
      motoristas(db),
      veiculos(db),
      viagens(db)
{
    ui->setupUi(this);
    configurarTabelas();
    configurarMascaras();

    connect(ui->btnVoltar, &QPushButton::clicked, this, &ViewViagens::voltarSolicitado);
    connect(ui->btnBuscarPaciente, &QPushButton::clicked, this, &ViewViagens::buscarPaciente);
    connect(ui->btnVerificarAcompanhante, &QPushButton::clicked, this, [this]
            { prepararAcompanhante(); });
    connect(ui->btnAdicionar, &QPushButton::clicked, this, &ViewViagens::adicionarPassageiro);
    connect(ui->btnRemover, &QPushButton::clicked, this, &ViewViagens::removerPassageiro);
    connect(ui->btnSalvar, &QPushButton::clicked, this, &ViewViagens::salvar);
    connect(ui->btnListar, &QPushButton::clicked, this, &ViewViagens::listar);
    connect(ui->tblViagens, &QTableWidget::cellClicked, this, &ViewViagens::carregarSelecionada);
    connect(ui->btnExcluir, &QPushButton::clicked, this, &ViewViagens::excluirViagem);
    connect(ui->btnLimpar, &QPushButton::clicked, this, &ViewViagens::limpar);
    connect(ui->txtCpfPaciente, &QLineEdit::returnPressed, this, &ViewViagens::buscarPaciente);
    connect(ui->txtCpfAcompanhante, &QLineEdit::returnPressed, this, [this]
            { prepararAcompanhante(); });

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

void ViewViagens::configurarMascaras()
{
    const auto formatarCampo = [](QLineEdit *campo, const QString &texto, const auto &formatador)
    {
        const QString formatado = formatador(texto);
        if (campo->text() == formatado)
            return;

        const QSignalBlocker bloqueio(campo);
        campo->setText(formatado);
        campo->setCursorPosition(formatado.size());
    };

    connect(ui->txtCpfPaciente, &QLineEdit::textChanged, this, [formatarCampo, this](const QString &texto)
            { formatarCampo(ui->txtCpfPaciente, texto, formatarCpf); });
    connect(ui->txtCpfAcompanhante, &QLineEdit::textChanged, this, [formatarCampo, this](const QString &texto)
            { formatarCampo(ui->txtCpfAcompanhante, texto, formatarCpf); });
    connect(ui->txtTelefoneAcompanhante, &QLineEdit::textChanged, this, [formatarCampo, this](const QString &texto)
            { formatarCampo(ui->txtTelefoneAcompanhante, texto, formatarTelefone); });
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
    else if (codigo == "P0001")
    {
        titulo = "Paciente ja possui viagem";
        detalhe = "Este paciente ja esta incluido em outra viagem na mesma data. Remova-o da viagem anterior antes de inclui-lo nesta.";
    }
    else if (codigo == "23505")
    {
        detalhe = "Há um vínculo de passageiro duplicado nesta viagem.";
    }
    else if (codigo == "23514" || codigo == "22001" || codigo == "22007")
    {
        detalhe = "Os dados não atendem às regras de integridade exigidas pelo sistema.";
    }
    else if (codigo == "42P01")
    {
        titulo = "Atualizacao do banco necessaria";
        detalhe = "A estrutura do banco esta desatualizada. Execute o arquivo "
                  "src/migracao_auxiliares_relatorios.sql e tente novamente.";
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
        ui->cmbAuxiliar->clear();
        ui->cmbAuxiliar->addItem("Sem auxiliar", 0);

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
        for (const auto &auxiliar : auxiliares.listarTodos())
        {
            ui->cmbAuxiliar->addItem(
                QString::fromStdString(auxiliar.nome + " - " + auxiliar.cpf), auxiliar.id);
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
    acompanhantePacientePendente.reset();
    acompanhanteAvulsoPendente.reset();

    ui->dtViagem->setDate(QDate::currentDate());
    ui->txtDestino->clear();
    ui->txtCpfPaciente->clear();
    ui->txtNomeAcompanhante->clear();
    ui->txtCpfAcompanhante->clear();
    ui->txtTelefoneAcompanhante->clear();
    ui->lblPaciente->clear();
    ui->lblAcompanhante->clear();
    ui->tblPassageiros->setRowCount(0);
    ui->lblMensagem->clear();
    ui->btnExcluir->setEnabled(false);
    atualizarCamposAcompanhante(false);
    carregarRecursos();
}

void ViewViagens::atualizarCamposAcompanhante(bool habilitado)
{
    ui->txtNomeAcompanhante->setEnabled(habilitado);
    ui->txtCpfAcompanhante->setEnabled(habilitado);
    ui->txtTelefoneAcompanhante->setEnabled(habilitado);
    ui->btnVerificarAcompanhante->setEnabled(habilitado);
}

void ViewViagens::buscarPaciente()
{
    pacientePendente.reset();
    acompanhantePacientePendente.reset();
    acompanhanteAvulsoPendente.reset();
    ui->lblPaciente->clear();
    ui->txtNomeAcompanhante->clear();
    ui->txtCpfAcompanhante->clear();
    ui->txtTelefoneAcompanhante->clear();
    ui->lblAcompanhante->clear();
    atualizarCamposAcompanhante(false);

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
        atualizarCamposAcompanhante(true);
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
    acompanhantePacientePendente.reset();
    ui->lblAcompanhante->clear();

    if (!pacientePendente.has_value())
    {
        erro("Busque um paciente antes de informar o acompanhante.");
        return;
    }

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
        if (acompanhantePendente.has_value() && acompanhantePendente->cpf == pacientePendente->cpf)
        {
            acompanhantePendente.reset();
            erro("O paciente não pode ser seu próprio acompanhante.");
            return;
        }
        if (!acompanhantePendente.has_value())
        {
            acompanhantePacientePendente = pacientes.buscarPorCPF(cpf);
            if (acompanhantePacientePendente.has_value())
            {
                if (acompanhantePacientePendente->id == pacientePendente->id)
                {
                    acompanhantePacientePendente.reset();
                    erro("O paciente não pode ser seu próprio acompanhante.");
                    return;
                }
                mensagem("Paciente localizado como acompanhante.");
                ui->lblAcompanhante->setText(
                    QString::fromStdString(acompanhantePacientePendente->nomeCompleto) + " (paciente cadastrado)");
                return;
            }
        }
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

bool ViewViagens::prepararAcompanhante()
{
    acompanhantePendente.reset();
    acompanhantePacientePendente.reset();
    acompanhanteAvulsoPendente.reset();
    ui->lblAcompanhante->clear();

    if (!pacientePendente.has_value())
    {
        erro("Busque um paciente antes de informar o acompanhante.");
        return false;
    }

    const QString nome = ui->txtNomeAcompanhante->text().trimmed();
    const QString cpfTexto = ui->txtCpfAcompanhante->text().trimmed();
    QString telefone = ui->txtTelefoneAcompanhante->text();
    telefone.remove(QRegularExpression("[^0-9]"));

    if (nome.isEmpty() && cpfTexto.isEmpty() && telefone.isEmpty())
        return true;

    const std::string cpf = CpfUtils::normalizar(cpfTexto.toStdString());
    if (!CpfUtils::verificar(cpf))
    {
        erro("Informe um CPF valido para o acompanhante.");
        return false;
    }

    try
    {
        acompanhantePacientePendente = pacientes.buscarPorCPF(cpf);
        if (acompanhantePacientePendente.has_value())
        {
            if (acompanhantePacientePendente->id == pacientePendente->id)
            {
                acompanhantePacientePendente.reset();
                erro("O paciente nao pode ser seu proprio acompanhante.");
                return false;
            }

            ui->txtNomeAcompanhante->setText(QString::fromStdString(acompanhantePacientePendente->nomeCompleto));
            ui->txtTelefoneAcompanhante->setText(QString::fromStdString(acompanhantePacientePendente->telefone));
            ui->lblAcompanhante->setText("Paciente cadastrado localizado como acompanhante.");
            mensagem("Acompanhante paciente localizado.");
            return true;
        }
    }
    catch (const pqxx::sql_error &erroBanco)
    {
        erroSql(erroBanco, "buscar o acompanhante");
        return false;
    }
    catch (const std::exception &)
    {
        erro("Nao foi possivel buscar o acompanhante. Verifique a conexao com o banco.");
        return false;
    }

    if (nome.isEmpty() || telefone.isEmpty())
    {
        erro("Para um acompanhante novo, informe nome, CPF e telefone.");
        return false;
    }
    if (!QRegularExpression("^[0-9]{10,11}$").match(telefone).hasMatch())
    {
        erro("O telefone do acompanhante deve ter DDD e 10 ou 11 digitos.");
        return false;
    }

    acompanhanteAvulsoPendente = AcompanhanteAvulso{nome.toStdString(), cpf, telefone.toStdString()};
    ui->lblAcompanhante->setText("Acompanhante informado para esta viagem.");
    mensagem("Acompanhante pronto para incluir na viagem.");
    return true;
}

void ViewViagens::adicionarPassageiro()
{
    if (pacientePendente.has_value() && !prepararAcompanhante())
        return;

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
    if (acompanhantePacientePendente.has_value())
    {
        detalhe.relacao.acompanhantePacienteId = acompanhantePacientePendente->id;
        detalhe.acompanhanteNome = acompanhantePacientePendente->nomeCompleto;
        detalhe.acompanhanteCpf = acompanhantePacientePendente->cpf;
        detalhe.acompanhanteTelefone = acompanhantePacientePendente->telefone;
    }
    else if (acompanhanteAvulsoPendente.has_value())
    {
        detalhe.relacao.acompanhanteAvulso = acompanhanteAvulsoPendente;
        detalhe.acompanhanteNome = acompanhanteAvulsoPendente->nomeCompleto;
        detalhe.acompanhanteCpf = acompanhanteAvulsoPendente->cpf;
        detalhe.acompanhanteTelefone = acompanhanteAvulsoPendente->telefone;
    }

    passageiros.push_back(detalhe);
    desenharPassageiros();
    pacientePendente.reset();
    acompanhantePendente.reset();
    acompanhantePacientePendente.reset();
    acompanhanteAvulsoPendente.reset();
    ui->txtCpfPaciente->clear();
    ui->txtNomeAcompanhante->clear();
    ui->txtCpfAcompanhante->clear();
    ui->txtTelefoneAcompanhante->clear();
    ui->lblPaciente->clear();
    ui->lblAcompanhante->clear();
    atualizarCamposAcompanhante(false);
    mensagem("Passageiro incluído na viagem.");
}

void ViewViagens::desenharPassageiros()
{
    std::sort(passageiros.begin(), passageiros.end(), [](const PassageiroViagemDetalhe &esquerda,
                                                         const PassageiroViagemDetalhe &direita)
              {
                  const int comparacao = QString::localeAwareCompare(QString::fromStdString(esquerda.pacienteNome),
                                                                       QString::fromStdString(direita.pacienteNome));
                  return comparacao == 0 ? esquerda.pacienteCpf < direita.pacienteCpf : comparacao < 0;
              });
    ui->tblPassageiros->setRowCount(static_cast<int>(passageiros.size()));
    for (int linha = 0; linha < static_cast<int>(passageiros.size()); ++linha)
    {
        const auto &passageiro = passageiros[linha];
        ui->tblPassageiros->setItem(linha, 0, new QTableWidgetItem(QString::fromStdString(passageiro.pacienteNome)));
        ui->tblPassageiros->setItem(linha, 1, new QTableWidgetItem(formatarCpf(QString::fromStdString(passageiro.pacienteCpf))));
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
    if (ui->cmbAuxiliar->currentData().toInt() > 0)
        viagem.auxiliarViagemId = ui->cmbAuxiliar->currentData().toInt();
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
        for (int indice = 0; indice < ui->cmbAuxiliar->count(); ++indice)
        {
            const int auxiliarId = detalhe->viagem.auxiliarViagemId.value_or(0);
            if (ui->cmbAuxiliar->itemData(indice).toInt() == auxiliarId)
                ui->cmbAuxiliar->setCurrentIndex(indice);
        }

        passageiros = detalhe->passageiros;
        desenharPassageiros();
        ui->btnExcluir->setEnabled(true);
        ui->btnExportarPlanilha->setEnabled(true);
        ui->btnGerarPdf->setEnabled(true);
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

void ViewViagens::exportarPlanilha()
{
    if (viagemId == 0)
    {
        erro("Selecione uma viagem antes de exportar.");
        return;
    }

    try
    {
        const auto relatorio = viagens.gerarRelatorioViagem(viagemId);
        if (!relatorio.has_value())
        {
            erro("A viagem selecionada não foi encontrada.");
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
        erro("Não foi possível exportar a planilha. Verifique a conexão com o banco.");
    }
}

void ViewViagens::gerarPdf()
{
    if (viagemId == 0)
    {
        erro("Selecione uma viagem antes de gerar o PDF.");
        return;
    }

    try
    {
        const auto relatorio = viagens.gerarRelatorioViagem(viagemId);
        if (!relatorio.has_value())
        {
            erro("A viagem selecionada não foi encontrada.");
            return;
        }

        QString caminho = QFileDialog::getSaveFileName(
            this, "Salvar relatório em PDF", "relatorio_viagem_" + QString::number(viagemId) + ".pdf",
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
        erro("Não foi possível gerar o PDF. Verifique a conexão com o banco.");
    }
}
