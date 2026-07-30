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

    try
    {
        // Valida o CPF antes de qualquer operação
        QString cpfDigitado = txtCpf->text();

        if (!CpfUtils::verificar(cpfDigitado.toStdString()))
        {
            QMessageBox::warning(this, "CPF Inválido", "O CPF informado não é válido. Por favor, verifique os números digitados.");
            return;
        }

        // Bloqueia o botão para evitar cliques duplos
        btnSalvar->setEnabled(false);

        // Monta o objeto Paciente
        Paciente p;
        p.nomeCompleto = txtNome->text().toStdString();
        p.cpf = cpfDigitado.toStdString();
        p.telefone = txtTelefone->text().toStdString();
        p.endereco = txtEndereco->text().toStdString();

        // Executa o cadastro (agora retorna o int diretamente ou lança exceção)
        int idGerado = m_repo.cadastrar(p);

        // Se chegou aqui, deu certo!
        QMessageBox::information(this, "Sucesso", "Paciente cadastrado com sucesso! ID: " + QString::number(idGerado));

        txtNome->clear();
        txtCpf->clear();
        txtTelefone->clear();
        txtEndereco->clear();

        btnSalvar->setEnabled(true);
    }
    catch (const pqxx::sql_error &e)
    {
        btnSalvar->setEnabled(true);

        // Tratamento específico para erros do PostgreSQL (como CPF duplicado ou FK Restrict)
        std::string sqlState = e.sqlstate();

        if (sqlState == "23505") // Código SQLSTATE para unique_violation (duplicidade)
        {
            QMessageBox::warning(this, "Atenção", "Este CPF já está cadastrado no sistema.");
        }
        else if (sqlState == "23503") // Código SQLSTATE para foreign_key_violation (restrição de exclusão/vínculo)
        {
            QMessageBox::warning(this, "Atenção", "Operação negada: existem registros vinculados a este item.");
        }
        else
        {
            QMessageBox::critical(this, "Erro no Banco de Dados", QString("Erro SQL: ") + e.what());
        }
    }
    catch (const std::exception &e)
    {
        btnSalvar->setEnabled(true);
        QMessageBox::critical(this, "Erro Crítico", QString("Ocorreu um erro: ") + e.what());
    }

    // Libera o botão novamente independente do que aconteceu
    btnSalvar->setEnabled(true);
}

void ViewPacientes::atualizarPaciente()
{
    try
    {
        QString cpfDigitado = txtCpf->text();

        // Valida o CPF
        if (!CpfUtils::verificar(cpfDigitado.toStdString()))
        {
            QMessageBox::warning(this, "CPF Inválido", "O CPF informado não é válido.");
            return;
        }

        // Bloqueia o botão durante o processamento
        btnAtualizar->setEnabled(false);

        Paciente p;
        p.nomeCompleto = txtNome->text().toStdString();
        p.cpf = cpfDigitado.toStdString();
        p.telefone = txtTelefone->text().toStdString();
        p.endereco = txtEndereco->text().toStdString();

        // Chama o método de atualização do repositório
        m_repo.atualizar(p);

        QMessageBox::information(this, "Sucesso", "Dados do paciente atualizados com sucesso!");

        btnAtualizar->setEnabled(true);
    }
    catch (const std::exception &e)
    {
        btnAtualizar->setEnabled(true);
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao atualizar: ") + e.what());
    }
}

void ViewPacientes::deletarPaciente()
{
    try
    {
        QString cpfDigitado = txtCpf->text();

        if (cpfDigitado.isEmpty())
        {
            QMessageBox::warning(this, "Atenção", "Informe o CPF do paciente que deseja excluir.");
            return;
        }

        // Pede confirmação antes de apagar
        QMessageBox::StandardButton resposta;
        resposta = QMessageBox::question(this, "Confirmar Exclusão",
                                         "Tem certeza que deseja excluir permanentemente este paciente?",
                                         QMessageBox::Yes | QMessageBox::No);

        if (resposta == QMessageBox::Yes)
        {
            btnDeletar->setEnabled(false);

            m_repo.deletar(cpfDigitado.toStdString());

            QMessageBox::information(this, "Sucesso", "Paciente excluído com sucesso!");

            // Limpa a tela após excluir
            txtNome->clear();
            txtCpf->clear();
            txtTelefone->clear();
            txtEndereco->clear();

            btnDeletar->setEnabled(true);
        }
    }
    catch (const pqxx::sql_error &e)
    {
        btnDeletar->setEnabled(true);
        std::string sqlState = e.sqlstate();

        // Trata a restrição do banco de dados (paciente já viajou)
        if (sqlState == "23503")
        {
            QMessageBox::warning(this, "Exclusão Negada", "Não é possível excluir este paciente pois ele possui viagens cadastradas no sistema.");
        }
        else
        {
            QMessageBox::critical(this, "Erro no Banco", QString("Erro SQL: ") + e.what());
        }
    }
    catch (const std::exception &e)
    {
        btnDeletar->setEnabled(true);
        QMessageBox::critical(this, "Erro Crítico", QString("Erro ao excluir: ") + e.what());
    }
}