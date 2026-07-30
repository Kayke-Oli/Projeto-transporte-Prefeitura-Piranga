#include "ViewPacientes.h"
#include "CpfUtils.h"
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
    txtCpf->setMaxLength(14); // Limita o tamanho máximo com a pontuação (11 números + 3 caracteres de pontuação)

    // Formata automaticamente conforme o usuário digita, sem poluir a tela antes
    connect(txtCpf, &QLineEdit::textChanged, this, [this](const QString &texto)
            {
    // Remove tudo que não for número para limpar a string
    QString numeros = texto;
    numeros.remove(QRegularExpression("[^\\d]"));

    // Limita a 11 dígitos numéricos reais
    if (numeros.length() > 11) {
        numeros = numeros.left(11);
    }

    QString formatado = "";
    int tamanho = numeros.length();

    // Aplica a máscara visual dinamicamente
    for (int i = 0; i < tamanho; ++i) {
        if (i == 3 || i == 6) {
            formatado += ".";
        } else if (i == 9) {
            formatado += "-";
        }
        formatado += numeros[i];
    }

    // Evita loop infinito bloqueando os sinais momentaneamente se o texto mudar
    QLineEdit *campo = qobject_cast<QLineEdit*>(QObject::sender());
    if (campo && campo->text() != formatado) {
        int cursor = campo->cursorPosition();
        campo->blockSignals(true);
        campo->setText(formatado);
        campo->blockSignals(false);
    } });

    txtTelefone = new QLineEdit(this);
    txtTelefone->setPlaceholderText("(00) 00000-0000");
    txtTelefone->setMaxLength(15); // Tamanho máximo com a formatação completa

    // Formata automaticamente o telefone no padrão (99) 99999-9999 conforme o usuário digita
    connect(txtTelefone, &QLineEdit::textChanged, this, [this](const QString &texto)
            {
    // Remove tudo que não for número
    QString numeros = texto;
    numeros.remove(QRegularExpression("[^\\d]"));

    // Limita a 11 dígitos numéricos (DDD + 9 dígitos do celular)
    if (numeros.length() > 11) {
        numeros = numeros.left(11);
    }

    QString formatado = "";
    int tamanho = numeros.length();

    // Aplica a máscara visual dinamicamente para o formato (99) 99999-9999
    for (int i = 0; i < tamanho; ++i) {
        if (i == 0) {
            formatado += "(";
        } else if (i == 2) {
            formatado += ") ";
        } else if (i == 7) {
            formatado += "-";
        }
        formatado += numeros[i];
    }

    // Atualiza o campo evitando loops infinitos de sinal
    QLineEdit *campo = qobject_cast<QLineEdit*>(QObject::sender());
    if (campo && campo->text() != formatado) {
        campo->blockSignals(true);
        campo->setText(formatado);
        campo->blockSignals(false);
    } });

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
    // Validação de Interface (Front-end)
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
    p.nomeCompleto = nome.toStdString();
    p.cpf = cpf.toStdString();
    p.telefone = txtTelefone->text().trimmed().toStdString();
    p.endereco = txtEndereco->text().trimmed().toStdString();

    try
    {

        // Valida usando CPFUtils
        if (!CpfUtils::verificar(p.cpf))
        {
            QMessageBox::warning(this, "CPF Inválido", "O CPF informado não é válido. Por favor, verifique os números digitados.");
            return; // Interrompe o cadastro na hora
        }

        // Faz o cadastro uma única vez
        auto idGerado = m_repo.cadastrar(p);

        // Bloqueia o botão para evitar cliques duplos
        btnSalvar->setEnabled(false);

        if (idGerado.has_value())
        {
            // Sucesso: avisa e limpa os campos da tela
            QMessageBox::information(this, "Sucesso", "Paciente cadastrado com sucesso! ID: " + QString::number(idGerado.value()));

            txtNome->clear();
            txtCpf->clear();
            txtTelefone->clear();
            txtEndereco->clear();
        }
        else
        {
            QMessageBox::warning(this, "Atenção", "Não foi possível cadastrar o paciente.\nPaciente já cadastrado ou CPF inexistente!");
        }

        // Reativa o botão no final
        btnSalvar->setEnabled(true);
    }
    catch (const std::exception &e)
    {
        btnSalvar->setEnabled(true);
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao cadastrar: ") + e.what());
    }

    // Libera o botão novamente independente do que aconteceu
    btnSalvar->setEnabled(true);
}