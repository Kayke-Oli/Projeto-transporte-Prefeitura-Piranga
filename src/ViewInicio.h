#pragma once
#include <QWidget>
class ViewInicio : public QWidget
{
    Q_OBJECT
public:
    explicit ViewInicio(QWidget *parent = nullptr);
signals:
    void pacientesSelecionado();
    void viagensSelecionado();
    void relatoriosSelecionado();
    void cadastrosSelecionado();

private:
    void configurarInterface();
};
