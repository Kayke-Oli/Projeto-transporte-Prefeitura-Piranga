#pragma once
#include <QWidget>
#include "ViagemRepository.h"
namespace Ui
{
    class ViewRelatorios;
}
class ViewRelatorios : public QWidget
{
    Q_OBJECT
public:
    explicit ViewRelatorios(Database &db, QWidget *parent = nullptr);
    ~ViewRelatorios();
    void prepararTela();
signals:
    void voltarSolicitado();
private slots:
    void historico();
    void volume();
    void mapa();
    void carregarViagensRelatorio();
    void exportarPlanilhaViagem();
    void gerarPdfViagem();

private:
    Ui::ViewRelatorios *ui;
    ViagemRepository repo;
    void configurarTabelas();
    int viagemRelatorioSelecionada() const;
    void mensagem(const QString &);
    void erro(const QString &);
    void erroSql(const pqxx::sql_error &, const QString &acao);
};
