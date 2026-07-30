#include "ViewPacientes.h"
#include "ui_ViewPacientes.h"
#include "CpfUtils.h"
#include <QMessageBox>
#include <QRegularExpression>

ViewPacientes::ViewPacientes(Database &db, ModoPaciente modo, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewPacientes), m_db(db), m_repo(db), m_modo(modo)
{
    ui->setupUi(this);

    connect(ui->btnSalvar, &QPushButton::clicked, this, &ViewPacientes::salvarPaciente);
    connect(ui->btnAtualizar, &QPushButton::clicked, this, &ViewPacientes::atualizarPaciente);
    connect(ui->btnDeletar, &QPushButton::clicked, this, &ViewPacientes::deletarPaciente);
    connect(ui->btnConsultar, &QPushButton::clicked, this, &ViewPacientes::buscarPaciente);
    connect(ui->btnVoltar, &QPushButton::clicked, this, &ViewPacientes::voltarSolicitado);
    connect(ui->listaResultados, &QListWidget::itemClicked, this, &ViewPacientes::selecionarResultado);

    configurarMascaras();
    configurarModo();
}

ViewPacientes::~ViewPacientes()
{
    delete ui;
}

void ViewPacientes::configurarModo()
{
    ui->btnSalvar->setVisible(m_modo == ModoPaciente::Cadastrar);
    ui->btnAtualizar->setVisible(m_modo == ModoPaciente::Atualizar);
    ui->btnDeletar->setVisible(m_modo == ModoPaciente::Excluir);

    bool temBusca = (m_modo == ModoPaciente::Atualizar || m_modo == ModoPaciente::Consultar);
    ui->btnConsultar->setVisible(temBusca);
    if (temBusca)
        ui->btnConsultar->setText("Buscar");

    // Excluir só precisa do CPF - o resto do formulário não tem função
    // nessa tela, então some.
    bool mostrarDadosCompletos = (m_modo != ModoPaciente::Excluir);
    ui->label->setVisible(mostrarDadosCompletos);
    ui->txtNome->setVisible(mostrarDadosCompletos);
    ui->label_3->setVisible(mostrarDadosCompletos);
    ui->txtTelefone->setVisible(mostrarDadosCompletos);
    ui->label_4->setVisible(mostrarDadosCompletos);
    ui->txtEndereco->setVisible(mostrarDadosCompletos);

    // Busca por nome só existe na Consulta
    ui->labelBuscaNome->setVisible(m_modo == ModoPaciente::Consultar);
    ui->txtBuscaNome->setVisible(m_modo == ModoPaciente::Consultar);
    ui->listaResultados->setVisible(false);

    bool somenteLeitura = (m_modo == ModoPaciente::Consultar);
    ui->txtNome->setReadOnly(somenteLeitura);
    ui->txtTelefone->setReadOnly(somenteLeitura);
    ui->txtEndereco->setReadOnly(somenteLeitura);

    // No modo Atualizar, os campos ficam travados até o usuário buscar o
    // cadastro existente pelo CPF - evita editar "no vazio" ou achar que
    // está criando um paciente novo.
    if (m_modo == ModoPaciente::Atualizar)
        habilitarCamposEdicao(false);

    switch (m_modo)
    {
    case ModoPaciente::Cadastrar:
        ui->labelTitulo->setText("Cadastrar Paciente");
        break;
    case ModoPaciente::Atualizar:
        ui->labelTitulo->setText("Atualizar Cadastro de Paciente");
        break;
    case ModoPaciente::Excluir:
        ui->labelTitulo->setText("Excluir Cadastro de Paciente");
        break;
    case ModoPaciente::Consultar:
        ui->labelTitulo->setText("Consultar Cadastro de Paciente");
        break;
    }
}

void ViewPacientes::habilitarCamposEdicao(bool habilitado)
{
    ui->txtNome->setEnabled(habilitado);
    ui->txtTelefone->setEnabled(habilitado);
    ui->txtEndereco->setEnabled(habilitado);
}

void ViewPacientes::preencherCampos(const Paciente &p)
{
    ui->txtCpf->setText(QString::fromStdString(p.cpf));
    ui->txtNome->setText(QString::fromStdString(p.nomeCompleto));
    ui->txtTelefone->setText(QString::fromStdString(p.telefone));
    ui->txtEndereco->setText(QString::fromStdString(p.endereco));
}

void ViewPacientes::limparCamposDados()
{
    ui->txtNome->clear();
    ui->txtTelefone->clear();
    ui->txtEndereco->clear();
}

void ViewPacientes::configurarMascaras()
{
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

void ViewPacientes::buscarPaciente()
{
    ui->listaResultados->clear();
    ui->listaResultados->setVisible(false);
    m_resultadosBusca.clear();

    QString cpfDigitado = ui->txtCpf->text().trimmed();

    if (!cpfDigitado.isEmpty())
    {
        auto pacienteEncontrado = m_repo.buscarPorCPF(cpfDigitado.toStdString());

        if (!pacienteEncontrado.has_value())
        {
            QMessageBox::information(this, "Não encontrado", "Nenhum paciente cadastrado com esse CPF.");
            limparCamposDados();
            if (m_modo == ModoPaciente::Atualizar)
                habilitarCamposEdicao(false);
            return;
        }

        preencherCampos(pacienteEncontrado.value());
        if (m_modo == ModoPaciente::Atualizar)
            habilitarCamposEdicao(true);
        return;
    }

    // Busca por nome só existe na tela de Consulta
    if (m_modo != ModoPaciente::Consultar)
    {
        QMessageBox::warning(this, "Atenção", "Informe o CPF do paciente.");
        return;
    }

    QString nomeBuscado = ui->txtBuscaNome->text().trimmed();

    if (nomeBuscado.isEmpty())
    {
        QMessageBox::warning(this, "Atenção", "Informe o CPF ou o nome do paciente que deseja consultar.");
        return;
    }

    auto resultados = m_repo.buscarPorNome(nomeBuscado.toStdString());

    if (resultados.empty())
    {
        QMessageBox::information(this, "Não encontrado", "Nenhum paciente encontrado com esse nome.");
        limparCamposDados();
        return;
    }

    if (resultados.size() == 1)
    {
        preencherCampos(resultados.front());
        return;
    }

    // Mais de um resultado - mostra a lista pra escolher
    m_resultadosBusca = resultados;
    for (const auto &p : resultados)
    {
        ui->listaResultados->addItem(
            QString::fromStdString(p.nomeCompleto) + " - " + QString::fromStdString(p.cpf));
    }
    ui->listaResultados->setVisible(true);
}

void ViewPacientes::selecionarResultado(QListWidgetItem *item)
{
    int indice = ui->listaResultados->row(item);
    if (indice < 0 || indice >= static_cast<int>(m_resultadosBusca.size()))
        return;

    preencherCampos(m_resultadosBusca[indice]);
    ui->listaResultados->setVisible(false);
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
            QMessageBox::warning(this, "Atenção", "Este CPF já está cadastrado.");
        else if (sqlState == "23503")
            QMessageBox::warning(this, "Atenção", "Operação negada: existem registros vinculados.");
        else
            QMessageBox::critical(this, "Erro no Banco", QString("Erro SQL: ") + e.what());
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

        // Volta a travar os campos - próxima edição exige nova busca,
        // pra não deixar a tela "aberta pra edição" indefinidamente.
        habilitarCamposEdicao(false);
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao atualizar: ") + e.what());
    }

    ui->btnAtualizar->setEnabled(true);
}

void ViewPacientes::deletarPaciente()
{
    QString cpfDigitado = ui->txtCpf->text().trimmed();

    if (cpfDigitado.isEmpty())
    {
        QMessageBox::warning(this, "Atenção", "Informe o CPF do paciente que deseja excluir.");
        return;
    }

    // Busca antes de excluir - assim a confirmação mostra o nome real,
    // em vez de pedir pra confirmar "excluir este CPF" às cegas.
    auto pacienteEncontrado = m_repo.buscarPorCPF(cpfDigitado.toStdString());

    if (!pacienteEncontrado.has_value())
    {
        QMessageBox::warning(this, "Não encontrado", "Nenhum paciente cadastrado com esse CPF.");
        return;
    }

    QString nomeEncontrado = QString::fromStdString(pacienteEncontrado->nomeCompleto);

    QMessageBox::StandardButton resposta = QMessageBox::question(
        this, "Confirmar Exclusão",
        QString("Deseja excluir %1?").arg(nomeEncontrado),
        QMessageBox::Yes | QMessageBox::No);

    if (resposta != QMessageBox::Yes)
        return;

    try
    {
        ui->btnDeletar->setEnabled(false);
        m_repo.deletar(cpfDigitado.toStdString());
        QMessageBox::information(this, "Sucesso", "Paciente excluído com sucesso!");
        ui->txtCpf->clear();
    }
    catch (const pqxx::sql_error &e)
    {
        std::string sqlState = e.sqlstate();
        if (sqlState == "23503")
            QMessageBox::warning(this, "Exclusão Negada", "Não é possível excluir este paciente pois ele possui viagens cadastradas.");
        else
            QMessageBox::critical(this, "Erro no Banco", QString("Erro SQL: ") + e.what());
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao excluir: ") + e.what());
    }

    ui->btnDeletar->setEnabled(true);
}