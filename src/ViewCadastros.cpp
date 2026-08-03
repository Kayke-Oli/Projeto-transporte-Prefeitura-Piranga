#include "ViewCadastros.h"
#include "CpfUtils.h"
#include "ui_ViewCadastros.h"
#include <QMessageBox>
#include <QRegularExpression>

namespace
{
    QString digitos(QString s)
    {
        s.remove(QRegularExpression("[^0-9]"));
        return s;
    }
    QString placa(QString s)
    {
        s.remove(QRegularExpression("[^A-Za-z0-9]"));
        return s.toUpper();
    }
}

ViewCadastros::ViewCadastros(Database &db, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewCadastros), acompanhantes(db), motoristas(db), veiculos(db)
{
    ui->setupUi(this);
    connect(ui->cmbTipo, qOverload<int>(&QComboBox::currentIndexChanged), this, &ViewCadastros::mudarTipo);
    connect(ui->btnBuscar, &QPushButton::clicked, this, &ViewCadastros::buscar);
    connect(ui->btnSalvar, &QPushButton::clicked, this, &ViewCadastros::salvar);
    connect(ui->btnAtualizar, &QPushButton::clicked, this, &ViewCadastros::atualizar);
    connect(ui->btnExcluir, &QPushButton::clicked, this, &ViewCadastros::excluirCadastro);
    connect(ui->btnLimpar, &QPushButton::clicked, this, &ViewCadastros::limpar);
    connect(ui->btnVoltar, &QPushButton::clicked, this, &ViewCadastros::voltarSolicitado);
    configurarTipo();
}
ViewCadastros::~ViewCadastros() { delete ui; }
void ViewCadastros::prepararTela() { limpar(); }
void ViewCadastros::mudarTipo()
{
    limpar();
    configurarTipo();
}
void ViewCadastros::configurarTipo()
{
    const int tipo = ui->cmbTipo->currentIndex();
    const bool veiculo = tipo == 2, motorista = tipo == 1;
    ui->lblChave->setText(veiculo ? "Placa *" : "CPF *");
    ui->lblNome->setText(veiculo ? "Modelo *" : "Nome completo *");
    ui->txtChave->setPlaceholderText(veiculo ? "ABC1D23 ou ABC-1234" : "000.000.000-00");
    ui->lblTelefone->setVisible(!veiculo && !motorista);
    ui->txtTelefone->setVisible(!veiculo && !motorista);
    ui->txtChave->setMaxLength(veiculo ? 8 : 14);
    ui->txtNome->setMaxLength(veiculo ? 100 : 150);
}
void ViewCadastros::limpar()
{
    selecionadoId = 0;
    ui->txtChave->clear();
    ui->txtNome->clear();
    ui->txtTelefone->clear();
    ui->txtChave->setReadOnly(false);
    ui->btnSalvar->setEnabled(true);
    ui->btnAtualizar->setEnabled(false);
    ui->btnExcluir->setEnabled(false);
    mensagem("");
    ui->txtChave->setFocus();
}
void ViewCadastros::mensagem(const QString &texto, bool erro)
{
    ui->lblMensagem->setStyleSheet(erro ? "color:#b42318;" : "color:#0f5132;");
    ui->lblMensagem->setText(texto);
}
bool ViewCadastros::validar()
{
    const int tipo = ui->cmbTipo->currentIndex();
    const QString chave = tipo == 2 ? placa(ui->txtChave->text()) : digitos(ui->txtChave->text());
    if (ui->txtNome->text().trimmed().isEmpty())
    {
        mensagem("Preencha o nome ou modelo.", true);
        return false;
    }
    if (tipo == 2)
    {
        if (!QRegularExpression("^[A-Z]{3}[0-9][A-Z0-9][0-9]{2}$").match(chave).hasMatch())
        {
            mensagem("Informe uma placa válida.", true);
            return false;
        }
    }
    else if (!CpfUtils::verificar(chave.toStdString()))
    {
        mensagem("Informe um CPF válido.", true);
        return false;
    }
    if (tipo == 0 && !ui->txtTelefone->text().isEmpty() && !QRegularExpression("^[0-9]{10,11}$").match(digitos(ui->txtTelefone->text())).hasMatch())
    {
        mensagem("Telefone deve ter DDD e 10 ou 11 dígitos.", true);
        return false;
    }
    return true;
}
void ViewCadastros::erroSql(const pqxx::sql_error &erro)
{
    const std::string codigo = erro.sqlstate();
    if (codigo == "23505")
    {
        mensagem("Já existe um cadastro com esta identificação.", true);
        QMessageBox::warning(this, "Cadastro duplicado", "Já existe um cadastro com esta identificação.");
    }
    else if (codigo == "23503")
    {
        mensagem("O cadastro possui viagens vinculadas e não pode ser excluído.", true);
        QMessageBox::warning(this, "Operação bloqueada", "Este cadastro possui viagens vinculadas e não pode ser excluído.");
    }
    else
    {
        mensagem("O banco rejeitou a operação. Revise os dados e tente novamente.", true);
        QMessageBox::critical(this, "Erro no banco", "Não foi possível concluir a operação.");
    }
}
void ViewCadastros::buscar()
{
    try
    {
        const int tipo = ui->cmbTipo->currentIndex();
        const QString chave = tipo == 2 ? placa(ui->txtChave->text()) : digitos(ui->txtChave->text());
        if (chave.isEmpty())
        {
            mensagem("Informe a identificação para buscar.", true);
            return;
        }
        if (tipo != 2 && !CpfUtils::verificar(chave.toStdString()))
        {
            mensagem("Informe um CPF válido.", true);
            return;
        }
        if (tipo == 2 && !QRegularExpression("^[A-Z]{3}[0-9][A-Z0-9][0-9]{2}$").match(chave).hasMatch())
        {
            mensagem("Informe uma placa válida.", true);
            return;
        }
        if (tipo == 0)
        {
            auto x = acompanhantes.buscarPorCPF(chave.toStdString());
            if (!x)
            {
                mensagem("Acompanhante não encontrado.", true);
                return;
            }
            selecionadoId = x->id;
            ui->txtNome->setText(QString::fromStdString(x->nomeCompleto));
            ui->txtTelefone->setText(QString::fromStdString(x->telefone));
        }
        else if (tipo == 1)
        {
            auto x = motoristas.buscarPorCPF(chave.toStdString());
            if (!x)
            {
                mensagem("Motorista não encontrado.", true);
                return;
            }
            selecionadoId = x->id;
            ui->txtNome->setText(QString::fromStdString(x->nome));
        }
        else
        {
            auto x = veiculos.buscarPorPlaca(chave.toStdString());
            if (!x)
            {
                mensagem("Veículo não encontrado.", true);
                return;
            }
            selecionadoId = x->id;
            ui->txtNome->setText(QString::fromStdString(x->modelo));
        }
        ui->txtChave->setText(chave);
        ui->txtChave->setReadOnly(true);
        ui->btnSalvar->setEnabled(false);
        ui->btnAtualizar->setEnabled(true);
        ui->btnExcluir->setEnabled(true);
        mensagem("Cadastro carregado.");
    }
    catch (const pqxx::sql_error &e)
    {
        erroSql(e);
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Conexão", "Não foi possível consultar o banco.");
    }
}
void ViewCadastros::salvar()
{
    if (!validar())
        return;
    ui->btnSalvar->setEnabled(false);
    try
    {
        const int tipo = ui->cmbTipo->currentIndex();
        const QString chave = tipo == 2 ? placa(ui->txtChave->text()) : digitos(ui->txtChave->text());
        int id = 0;
        if (tipo == 0)
            id = acompanhantes.cadastrar(Acompanhante{0, ui->txtNome->text().trimmed().toStdString(), chave.toStdString(), digitos(ui->txtTelefone->text()).toStdString()});
        else if (tipo == 1)
            id = motoristas.cadastrar(Motorista{0, ui->txtNome->text().trimmed().toStdString(), chave.toStdString()});
        else
            id = veiculos.cadastrar(Veiculo{0, chave.toStdString(), ui->txtNome->text().trimmed().toStdString()});
        limpar();
        mensagem("Cadastro salvo com sucesso. Código: " + QString::number(id));
    }
    catch (const pqxx::sql_error &e)
    {
        erroSql(e);
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Conexão", "Não foi possível salvar o cadastro.");
    }

    if (selecionadoId == 0)
        ui->btnSalvar->setEnabled(true);
}
void ViewCadastros::atualizar()
{
    if (selecionadoId == 0 || !validar())
        return;
    ui->btnAtualizar->setEnabled(false);
    try
    {
        const int tipo = ui->cmbTipo->currentIndex();
        bool ok = false;
        if (tipo == 0)
            ok = acompanhantes.atualizar(Acompanhante{selecionadoId, ui->txtNome->text().trimmed().toStdString(), digitos(ui->txtChave->text()).toStdString(), digitos(ui->txtTelefone->text()).toStdString()});
        else if (tipo == 1)
            ok = motoristas.atualizar(Motorista{selecionadoId, ui->txtNome->text().trimmed().toStdString(), digitos(ui->txtChave->text()).toStdString()});
        else
            ok = veiculos.atualizar(Veiculo{selecionadoId, placa(ui->txtChave->text()).toStdString(), ui->txtNome->text().trimmed().toStdString()});
        if (!ok)
        {
            limpar();
            QMessageBox::warning(this, "Cadastro não encontrado", "O registro não existe mais.");
            return;
        }
        limpar();
        mensagem("Cadastro atualizado com sucesso.");
    }
    catch (const pqxx::sql_error &e)
    {
        erroSql(e);
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Conexão", "Não foi possível atualizar o cadastro.");
    }

    if (selecionadoId != 0)
        ui->btnAtualizar->setEnabled(true);
}
void ViewCadastros::excluirCadastro()
{
    if (selecionadoId == 0)
        return;
    if (QMessageBox::question(this, "Confirmar exclusão", "Excluir definitivamente este cadastro?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;
    ui->btnExcluir->setEnabled(false);
    try
    {
        const int tipo = ui->cmbTipo->currentIndex();
        const bool ok = tipo == 0 ? acompanhantes.deletar(selecionadoId) : (tipo == 1 ? motoristas.deletar(selecionadoId) : veiculos.deletar(selecionadoId));
        if (!ok)
            QMessageBox::warning(this, "Cadastro não encontrado", "O registro não existe mais.");
        else
        {
            limpar();
            mensagem("Cadastro excluído com sucesso.");
        }
    }
    catch (const pqxx::sql_error &e)
    {
        erroSql(e);
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Conexão", "Não foi possível excluir o cadastro.");
    }

    if (selecionadoId != 0)
        ui->btnExcluir->setEnabled(true);
}
