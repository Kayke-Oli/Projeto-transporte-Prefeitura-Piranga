#include "ViewPacientes.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

ViewPacientes::ViewPacientes(Database &db, QWidget *parent)
    : QWidget(parent), m_db(db), m_repo(db)
{
    configurarFormulario();
}

void ViewPacientes::configurarFormulario()
{
    // QVBoxLayout principal que gerencia o formulário e futuros componentes (como tabelas)
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);

    // QFormLayout é perfeito para alinhar labels e campos automaticamente
    QFormLayout *formLayout = new QFormLayout();

    txtNome = new QLineEdit(this);
    txtNome->setPlaceholderText("Nome completo do paciente");
    txtNome->setMaxLength(100);

    txtCpf = new QLineEdit(this);
    txtCpf->setPlaceholderText("000.000.000-00");
    // Validação rígida: Só aceita no formato exato de CPF. Evita crash no SQL.
    QRegularExpression regexCpf("\\d{3}\\.\\d{3}\\.\\d{3}-\\d{2}");
    QValidator *validadorCpf = new QRegularExpressionValidator(regexCpf, this);
    txtCpf->setValidator(validadorCpf);

    txtTelefone = new QLineEdit(this);
    txtTelefone->setPlaceholderText("(00) 00000-0000");

    txtEndereco = new QLineEdit(this);

    btnSalvar = new QPushButton("Cadastrar Paciente", this);
    btnSalvar->setMinimumHeight(40);
    btnSalvar->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; border-radius: 4px;");

    formLayout->addRow("Nome Completo:", txtNome);
    formLayout->addRow("CPF:", txtCpf);
    formLayout->addRow("Telefone:", txtTelefone);
    formLayout->addRow("Endereço:", txtEndereco);

    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(btnSalvar);
    layoutPrincipal->addStretch(); // Empurra tudo para cima, não deixa esticar os campos

    // Conecta o clique do botão à função de salvar
    connect(btnSalvar, &QPushButton::clicked, this, &ViewPacientes::salvarPaciente);
}

void ViewPacientes::salvarPaciente()
{
    // 1. Validação de Interface (Front-end)
    QString nome = txtNome->text().trimmed();
    QString cpf = txtCpf->text().trimmed();

    if (nome.isEmpty() || cpf.isEmpty())
    {
        QMessageBox::warning(this, "Atenção", "Os campos Nome e CPF são obrigatórios.");
        return;
    }

    if (!txtCpf->hasAcceptableInput())
    {
        QMessageBox::warning(this, "Atenção", "O CPF digitado está incompleto ou inválido.");
        return;
    }

    // 2. Transição segura para o Domínio/Banco (Back-end)
    Paciente p;
    p.nome = nome.toStdString();
    p.cpf = cpf.toStdString();
    p.telefone = txtTelefone->text().trimmed().toStdString();
    p.endereco = txtEndereco->text().trimmed().toStdString();

    try
    {
        // Bloqueia o botão para evitar cliques duplos que geram duplicidade
        btnSalvar->setEnabled(false);

        m_repo.cadastrar(p); // Aqui o PostgreSQL garante a transação

        QMessageBox::information(this, "Sucesso", "Paciente cadastrado com sucesso no sistema!");

        // Limpa os campos
        txtNome->clear();
        txtCpf->clear();
        txtTelefone->clear();
        txtEndereco->clear();
    }
    catch (const std::exception &e)
    {
        // Trata erro de violação de UNIQUE (CPF repetido) ou queda de banco
        QMessageBox::critical(this, "Erro de Banco de Dados",
                              QString("Não foi possível salvar o paciente.\nDetalhes: ") + e.what());
    }

    // Libera o botão novamente independente do que aconteceu
    btnSalvar->setEnabled(true);
}