#pragma once

#include <QWidget>
#include "AcompanhanteRepository.h"
#include "Motoristarepository.h"
#include "VeiculoRepository.h"

namespace Ui
{
    class ViewCadastros;
}

class ViewCadastros : public QWidget
{
    Q_OBJECT
public:
    explicit ViewCadastros(Database &db, QWidget *parent = nullptr);
    ~ViewCadastros();
    void prepararTela();
signals:
    void voltarSolicitado();
private slots:
    void mudarTipo();
    void buscar();
    void salvar();
    void atualizar();
    void excluirCadastro();
    void limpar();

private:
    Ui::ViewCadastros *ui;
    AcompanhanteRepository acompanhantes;
    MotoristaRepository motoristas;
    VeiculoRepository veiculos;
    int selecionadoId = 0;
    void configurarTipo();
    bool validar();
    void mensagem(const QString &texto, bool erro = false);
    void erroSql(const pqxx::sql_error &erro);
};
