#include "ViewPacientes.h"
#include "CpfUtils.h"
#include "ui_ViewPacientes.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QStringList>
#include <QTableWidgetItem>

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
        for (qsizetype i = 0; i < numeros.size(); ++i)
        {
            if (i == 3 || i == 6)
                resultado += ".";
            else if (i == 9)
                resultado += "-";
            resultado += numeros[i];
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

    QString cpfFormatado(const std::string &cpf)
    {
        return formatarCpf(QString::fromStdString(cpf));
    }

    QString telefoneFormatado(const std::string &telefone)
    {
        return formatarTelefone(QString::fromStdString(telefone));
    }
}

ViewPacientes::ViewPacientes(Database &db, ModoPaciente modo, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewPacientes), m_db(db), m_repo(db), m_modo(modo)
{
    ui->setupUi(this);

    connect(ui->btnSalvar, &QPushButton::clicked, this, &ViewPacientes::salvarPaciente);
    connect(ui->btnAtualizar, &QPushButton::clicked, this, &ViewPacientes::atualizarPaciente);
    connect(ui->btnDeletar, &QPushButton::clicked, this, &ViewPacientes::deletarPaciente);
    connect(ui->btnConsultar, &QPushButton::clicked, this, &ViewPacientes::buscarPaciente);
    connect(ui->btnLimpar, &QPushButton::clicked, this, &ViewPacientes::limparFormulario);
    connect(ui->btnVoltar, &QPushButton::clicked, this, &ViewPacientes::voltarSolicitado);
    connect(ui->tabelaResultados, &QTableWidget::cellClicked, this, &ViewPacientes::selecionarResultado);
    connect(ui->txtCpf, &QLineEdit::returnPressed, this, [this]
            {
                if (m_modo != ModoPaciente::Cadastrar)
                    buscarPaciente(); });
    connect(ui->txtBuscaNome, &QLineEdit::returnPressed, this, &ViewPacientes::buscarPaciente);

    configurarMascaras();
    configurarModo();
    prepararTela();
}

ViewPacientes::~ViewPacientes()
{
    delete ui;
}

void ViewPacientes::prepararTela()
{
    limparFormulario();
}

void ViewPacientes::configurarModo()
{
    ui->btnSalvar->setVisible(m_modo == ModoPaciente::Cadastrar);
    ui->btnAtualizar->setVisible(m_modo == ModoPaciente::Atualizar);
    ui->btnDeletar->setVisible(m_modo == ModoPaciente::Excluir);

    const bool temBuscaPorCpf = m_modo != ModoPaciente::Cadastrar;
    ui->btnConsultar->setVisible(temBuscaPorCpf);
    ui->btnConsultar->setText(m_modo == ModoPaciente::Consultar ? "Buscar" : "Buscar por CPF");
    ui->grupoBuscaNome->setVisible(m_modo == ModoPaciente::Consultar);

    ui->tabelaResultados->setColumnCount(3);
    ui->tabelaResultados->setHorizontalHeaderLabels(QStringList{"Nome", "CPF", "Telefone"});
    ui->tabelaResultados->horizontalHeader()->setStretchLastSection(true);
    ui->tabelaResultados->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tabelaResultados->setColumnWidth(1, 125);
    ui->tabelaResultados->setColumnWidth(2, 135);

    ui->txtNome->setReadOnly(m_modo == ModoPaciente::Consultar || m_modo == ModoPaciente::Excluir);
    ui->txtTelefone->setReadOnly(m_modo == ModoPaciente::Consultar || m_modo == ModoPaciente::Excluir);
    ui->txtEndereco->setReadOnly(m_modo == ModoPaciente::Consultar || m_modo == ModoPaciente::Excluir);

    switch (m_modo)
    {
    case ModoPaciente::Cadastrar:
        ui->labelTitulo->setText("Cadastrar paciente");
        ui->labelInstrucao->setText("Informe os dados do paciente. Nome e CPF são obrigatórios.");
        break;
    case ModoPaciente::Atualizar:
        ui->labelTitulo->setText("Atualizar cadastro de paciente");
        ui->labelInstrucao->setText("Busque pelo CPF antes de editar. O CPF não é alterado neste fluxo.");
        break;
    case ModoPaciente::Excluir:
        ui->labelTitulo->setText("Excluir cadastro de paciente");
        ui->labelInstrucao->setText("Busque e confira o paciente antes de confirmar a exclusão.");
        break;
    case ModoPaciente::Consultar:
        ui->labelTitulo->setText("Consultar cadastro de paciente");
        ui->labelInstrucao->setText("Busque por CPF ou informe pelo menos três letras do nome.");
        break;
    }
}

void ViewPacientes::configurarMascaras()
{
    connect(ui->txtCpf, &QLineEdit::textChanged, this, [this](const QString &texto)
            {
                const QString formatado = formatarCpf(texto);
                if (ui->txtCpf->text() == formatado)
                    return;

                const QSignalBlocker bloqueio(ui->txtCpf);
                ui->txtCpf->setText(formatado);
                ui->txtCpf->setCursorPosition(formatado.size()); });

    connect(ui->txtTelefone, &QLineEdit::textChanged, this, [this](const QString &texto)
            {
                const QString formatado = formatarTelefone(texto);
                if (ui->txtTelefone->text() == formatado)
                    return;

                const QSignalBlocker bloqueio(ui->txtTelefone);
                ui->txtTelefone->setText(formatado);
                ui->txtTelefone->setCursorPosition(formatado.size()); });
}

void ViewPacientes::habilitarCamposEdicao(bool habilitado)
{
    ui->txtNome->setEnabled(habilitado);
    ui->txtTelefone->setEnabled(habilitado);
    ui->txtEndereco->setEnabled(habilitado);
}

void ViewPacientes::preencherCampos(const Paciente &paciente)
{
    ui->txtCpf->setText(cpfFormatado(paciente.cpf));
    ui->txtNome->setText(QString::fromStdString(paciente.nomeCompleto));
    ui->txtTelefone->setText(telefoneFormatado(paciente.telefone));
    ui->txtEndereco->setText(QString::fromStdString(paciente.endereco));
}

void ViewPacientes::selecionarPaciente(const Paciente &paciente)
{
    m_pacienteSelecionadoId = paciente.id;
    preencherCampos(paciente);
    ui->tabelaResultados->setVisible(false);
    ui->txtCpf->setReadOnly(m_modo != ModoPaciente::Cadastrar);

    if (m_modo == ModoPaciente::Atualizar)
    {
        habilitarCamposEdicao(true);
        ui->btnAtualizar->setEnabled(true);
        ui->txtNome->setFocus();
        ui->labelMensagem->setStyleSheet("color: #0f5132;");
        ui->labelMensagem->setText("Cadastro carregado. Revise os dados e confirme a atualização.");
    }
    else if (m_modo == ModoPaciente::Excluir)
    {
        habilitarCamposEdicao(false);
        ui->btnDeletar->setEnabled(true);
        ui->labelMensagem->setStyleSheet("color: #8a1c1c;");
        ui->labelMensagem->setText("Confira os dados. A exclusão não poderá ser desfeita.");
    }
    else if (m_modo == ModoPaciente::Consultar)
    {
        ui->labelMensagem->setStyleSheet("color: #0f5132;");
        ui->labelMensagem->setText("Cadastro encontrado.");
    }
}

void ViewPacientes::limparCamposDados()
{
    ui->txtNome->clear();
    ui->txtTelefone->clear();
    ui->txtEndereco->clear();
}

void ViewPacientes::limparFormulario()
{
    m_pacienteSelecionadoId = 0;
    m_resultadosBusca.clear();
    ui->tabelaResultados->clearContents();
    ui->tabelaResultados->setRowCount(0);
    ui->tabelaResultados->setVisible(false);
    ui->txtCpf->clear();
    ui->txtBuscaNome->clear();
    limparCamposDados();
    limparMensagemValidacao();

    ui->txtCpf->setReadOnly(false);

    if (m_modo == ModoPaciente::Cadastrar)
    {
        habilitarCamposEdicao(true);
        ui->btnSalvar->setEnabled(true);
        ui->txtNome->setFocus();
    }
    else if (m_modo == ModoPaciente::Atualizar)
    {
        habilitarCamposEdicao(false);
        ui->btnAtualizar->setEnabled(false);
        ui->txtCpf->setFocus();
    }
    else if (m_modo == ModoPaciente::Excluir)
    {
        habilitarCamposEdicao(false);
        ui->btnDeletar->setEnabled(false);
        ui->txtCpf->setFocus();
    }
    else
    {
        habilitarCamposEdicao(true);
        ui->txtNome->setReadOnly(true);
        ui->txtTelefone->setReadOnly(true);
        ui->txtEndereco->setReadOnly(true);
        ui->txtCpf->setFocus();
    }
}

void ViewPacientes::exibirMensagemValidacao(const QString &mensagem, QWidget *campo)
{
    ui->labelMensagem->setStyleSheet("color: #b42318;");
    ui->labelMensagem->setText(mensagem);
    if (campo)
        campo->setFocus();
}

void ViewPacientes::limparMensagemValidacao()
{
    ui->labelMensagem->clear();
    ui->labelMensagem->setStyleSheet("color: #475467;");
}

bool ViewPacientes::validarDadosPaciente()
{
    limparMensagemValidacao();

    if (ui->txtNome->text().trimmed().isEmpty())
    {
        exibirMensagemValidacao("Informe o nome completo do paciente.", ui->txtNome);
        return false;
    }

    const std::string cpf = CpfUtils::normalizar(ui->txtCpf->text().toStdString());
    if (!CpfUtils::verificar(cpf))
    {
        exibirMensagemValidacao("Informe um CPF válido.", ui->txtCpf);
        return false;
    }

    const QString telefone = somenteDigitos(ui->txtTelefone->text());
    if (!telefone.isEmpty() && telefone.size() != 10 && telefone.size() != 11)
    {
        exibirMensagemValidacao("O telefone deve ter DDD e 10 ou 11 dígitos.", ui->txtTelefone);
        return false;
    }

    return true;
}

void ViewPacientes::mostrarErroBanco(const pqxx::sql_error &erro, const QString &acao)
{
    const std::string codigo = erro.sqlstate();
    if (codigo == "23505")
    {
        QMessageBox::warning(this, "CPF já cadastrado", "Já existe um paciente cadastrado com este CPF.");
    }
    else if (codigo == "23503")
    {
        QMessageBox::warning(this, "Operação não permitida",
                             "Não é possível concluir a operação porque existem viagens vinculadas a este paciente.");
    }
    else if (codigo == "23514" || codigo == "22001")
    {
        QMessageBox::warning(this, "Dados inválidos",
                             "Os dados informados não atendem às regras de integridade do cadastro.");
    }
    else
    {
        QMessageBox::critical(this, "Erro no banco de dados",
                              QString("Não foi possível %1. Tente novamente ou contate o suporte.").arg(acao));
    }
}

void ViewPacientes::buscarPaciente()
{
    limparMensagemValidacao();
    m_resultadosBusca.clear();
    ui->tabelaResultados->clearContents();
    ui->tabelaResultados->setRowCount(0);
    ui->tabelaResultados->setVisible(false);

    try
    {
        const QString cpfDigitado = ui->txtCpf->text().trimmed();
        if (!cpfDigitado.isEmpty())
        {
            const std::string cpf = CpfUtils::normalizar(cpfDigitado.toStdString());
            if (!CpfUtils::verificar(cpf))
            {
                exibirMensagemValidacao("Informe um CPF válido para realizar a busca.", ui->txtCpf);
                return;
            }

            const auto paciente = m_repo.buscarPorCPF(cpf);
            if (!paciente.has_value())
            {
                limparCamposDados();
                exibirMensagemValidacao("Nenhum paciente foi encontrado com este CPF.", ui->txtCpf);
                return;
            }

            selecionarPaciente(*paciente);
            return;
        }

        if (m_modo != ModoPaciente::Consultar)
        {
            exibirMensagemValidacao("Informe o CPF do paciente que deseja localizar.", ui->txtCpf);
            return;
        }

        const QString nomeBuscado = ui->txtBuscaNome->text().trimmed();
        if (nomeBuscado.size() < 3)
        {
            exibirMensagemValidacao("Informe pelo menos três letras do nome para pesquisar.", ui->txtBuscaNome);
            return;
        }

        m_resultadosBusca = m_repo.buscarPorNome(nomeBuscado.toStdString());
        if (m_resultadosBusca.empty())
        {
            limparCamposDados();
            exibirMensagemValidacao("Nenhum paciente foi encontrado com este nome.", ui->txtBuscaNome);
            return;
        }

        if (m_resultadosBusca.size() == 1)
        {
            selecionarPaciente(m_resultadosBusca.front());
            return;
        }

        ui->tabelaResultados->setRowCount(static_cast<int>(m_resultadosBusca.size()));
        for (int linha = 0; linha < static_cast<int>(m_resultadosBusca.size()); ++linha)
        {
            const Paciente &paciente = m_resultadosBusca[linha];
            auto *itemNome = new QTableWidgetItem(QString::fromStdString(paciente.nomeCompleto));
            itemNome->setData(Qt::UserRole, paciente.id);
            ui->tabelaResultados->setItem(linha, 0, itemNome);
            ui->tabelaResultados->setItem(linha, 1, new QTableWidgetItem(cpfFormatado(paciente.cpf)));
            ui->tabelaResultados->setItem(linha, 2, new QTableWidgetItem(telefoneFormatado(paciente.telefone)));
        }

        ui->tabelaResultados->setVisible(true);
        ui->labelMensagem->setStyleSheet("color: #475467;");
        ui->labelMensagem->setText(m_resultadosBusca.size() == 20
                                       ? "Exibindo os 20 primeiros resultados. Refine a busca, se necessário."
                                       : "Selecione um paciente na tabela para ver o cadastro.");
    }
    catch (const pqxx::sql_error &erro)
    {
        mostrarErroBanco(erro, "consultar o paciente");
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Erro de conexão",
                              "Não foi possível consultar o banco de dados. Verifique a conexão e tente novamente.");
    }
}

void ViewPacientes::selecionarResultado(int linha, int)
{
    if (linha < 0 || linha >= static_cast<int>(m_resultadosBusca.size()))
        return;

    selecionarPaciente(m_resultadosBusca[linha]);
}

void ViewPacientes::salvarPaciente()
{
    if (!validarDadosPaciente())
        return;

    ui->btnSalvar->setEnabled(false);
    try
    {
        Paciente paciente;
        paciente.nomeCompleto = ui->txtNome->text().trimmed().toStdString();
        paciente.cpf = CpfUtils::normalizar(ui->txtCpf->text().toStdString());
        paciente.telefone = somenteDigitos(ui->txtTelefone->text()).toStdString();
        paciente.endereco = ui->txtEndereco->text().trimmed().toStdString();

        const int id = m_repo.cadastrar(paciente);
        prepararTela();
        ui->labelMensagem->setStyleSheet("color: #0f5132;");
        ui->labelMensagem->setText("Paciente cadastrado com sucesso. Código: " + QString::number(id) + ".");
    }
    catch (const pqxx::sql_error &erro)
    {
        mostrarErroBanco(erro, "cadastrar o paciente");
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Erro de conexão",
                              "Não foi possível cadastrar o paciente. Verifique a conexão e tente novamente.");
    }

    ui->btnSalvar->setEnabled(true);
}

void ViewPacientes::atualizarPaciente()
{
    if (m_pacienteSelecionadoId == 0)
    {
        exibirMensagemValidacao("Busque um paciente antes de atualizar o cadastro.", ui->txtCpf);
        return;
    }
    if (!validarDadosPaciente())
        return;

    ui->btnAtualizar->setEnabled(false);
    try
    {
        Paciente paciente;
        paciente.id = m_pacienteSelecionadoId;
        paciente.nomeCompleto = ui->txtNome->text().trimmed().toStdString();
        paciente.cpf = CpfUtils::normalizar(ui->txtCpf->text().toStdString());
        paciente.telefone = somenteDigitos(ui->txtTelefone->text()).toStdString();
        paciente.endereco = ui->txtEndereco->text().trimmed().toStdString();

        if (!m_repo.atualizar(paciente))
        {
            prepararTela();
            QMessageBox::warning(this, "Cadastro não encontrado",
                                 "O paciente foi removido por outro atendimento. Faça uma nova busca.");
            return;
        }

        prepararTela();
        ui->labelMensagem->setStyleSheet("color: #0f5132;");
        ui->labelMensagem->setText("Cadastro atualizado com sucesso.");
    }
    catch (const pqxx::sql_error &erro)
    {
        mostrarErroBanco(erro, "atualizar o paciente");
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Erro de conexão",
                              "Não foi possível atualizar o paciente. Verifique a conexão e tente novamente.");
    }

    if (m_pacienteSelecionadoId != 0)
        ui->btnAtualizar->setEnabled(true);
}

void ViewPacientes::deletarPaciente()
{
    if (m_pacienteSelecionadoId == 0)
    {
        exibirMensagemValidacao("Busque um paciente antes de solicitar a exclusão.", ui->txtCpf);
        return;
    }

    const QMessageBox::StandardButton resposta = QMessageBox::question(
        this, "Confirmar exclusão",
        "Deseja excluir definitivamente o cadastro de " + ui->txtNome->text() + "?\n\n"
                                                                                "Esta ação não poderá ser desfeita.",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (resposta != QMessageBox::Yes)
        return;

    ui->btnDeletar->setEnabled(false);
    try
    {
        if (!m_repo.deletar(m_pacienteSelecionadoId))
        {
            prepararTela();
            QMessageBox::warning(this, "Cadastro não encontrado",
                                 "O paciente foi removido por outro atendimento. Faça uma nova busca.");
            return;
        }

        prepararTela();
        ui->labelMensagem->setStyleSheet("color: #0f5132;");
        ui->labelMensagem->setText("Cadastro excluído com sucesso.");
    }
    catch (const pqxx::sql_error &erro)
    {
        mostrarErroBanco(erro, "excluir o paciente");
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, "Erro de conexão",
                              "Não foi possível excluir o paciente. Verifique a conexão e tente novamente.");
    }

    if (m_pacienteSelecionadoId != 0)
        ui->btnDeletar->setEnabled(true);
}
